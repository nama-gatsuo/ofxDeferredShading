#include "DofPass.hpp"

using namespace ofxDeferred;

DofPass::DofPass(const glm::vec2& size) :
	RenderPass(size, RenderPassRegistry::Dof), blur(size / 4., GL_RGBA),
	atomicBuffer(4)
{

	ofFboSettings s;
	s.width = size.x / 4.;
	s.height = size.y / 4.;
	s.internalformat = GL_RGBA;
	s.numColorbuffers = 1;
	s.numSamples = 1;
	//s.minFilter = GL_NEAREST;
	//s.maxFilter = GL_NEAREST;
	s.useDepth = false;
	s.useStencil = false;

	shrunk.allocate(s);
	shrunkBlurred.allocate(s);
	nearCoC.allocate(s);
	colorBlurred.allocate(s);

	s.width = size.x;
	s.height = size.y;
	s.internalformat = GL_R32F;
	depth.allocate(s);
	depth.getTexture().setRGToRGBASwizzles(true);
	
	s.width = 256;
	s.height = 256;
	s.internalformat = GL_R8;
	bokehShapeTex.allocate(256, 256, GL_R8);

	downSample.load(passThruPath, shaderPath + "dof/downSample.frag");
	calcNearCoc.load(passThruPath, shaderPath + "dof/calcNearCoc.frag");
	smallBlur.load(passThruPath, shaderPath + "dof/smallBlur.frag");
	applyDof.load(passThruPath, shaderPath + "dof/applyDof.frag");
	debugShader.load(passThruPath, shaderPath + "alphaFrag.frag");
	blur.setPreShrink(1);

	detectBokehShader.loadCompute(shaderPath + "bokeh/bokehCalc.glsl");
	bokehRenderShader.load(shaderPath + "bokeh/bokehRender");
	bokehShapingShader.load(passThruPath, shaderPath + "bokeh/bokehShaping.frag");

	group.add(endPointsCoC.set("endpoint_coc", glm::vec2(0.9, 0.6), glm::vec2(0.), glm::vec2(1.)));
	group.add(foculRange.set("focul_range", glm::vec2(0.1, 0.3), glm::vec2(0.), glm::vec2(1.)));
	group.add(blur.getParameters());

	bokehGroup.setName("bokeh");
	bokehGroup.add(isActiveBokeh.set("isActiveBokeh", false));
	bokehGroup.add(maxBokehCount.set("maxBokehCount", 500, 1, 1000));
	bokehGroup.add(bokehCocThres.set("bokehCocThres", 0.03, 0., 1.));
	bokehGroup.add(bokehLumThres.set("bokehLumThres", 0.03, 0., 10.));
	bokehGroup.add(maxBokehRadius.set("maxBokehRadius", 100., 0., 300.));
	bokehGroup.add(bokehDepthCutoff.set("bokehDepthCutoff", 0.03, 0.0, 1.0));
	bokehGroup.add(bokehShapeId.set("bokehShapeId", 0, 0, 2));
	group.add(bokehGroup);

	// Add listeners
	bokehShapeId.addListener(this, &DofPass::onTexParamChanged);
	maxBokehCount.addListener(this, &DofPass::onMaxBokehCountChanged);
	int i = 0;
	onTexParamChanged(i);
	onMaxBokehCountChanged(i);

	vbo = ofMesh::plane(1., 1.);
}

void DofPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {

	depth.begin();
	ofClear(255);
	debugShader.begin();
	gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL).draw(0, 0);
	debugShader.end();
	depth.end();

	ofDisableAlphaBlending();
	// downSample and initialize CoC
	shrunk.begin();
	ofClear(0);
	{
		downSample.begin();
		downSample.setUniformTexture("colorTex", read, 1);
		downSample.setUniformTexture("depthTex", depth.getTexture(), 2);
		downSample.setUniform1f("nearEndCoc", endPointsCoC.get().x);
		downSample.setUniform1f("foculRangeStart", foculRange.get().x);
		shrunk.draw(0, 0);
		downSample.end();
	}
	shrunk.end();

	// blur downSampled CoC image
	blur.render(shrunk.getTexture(), shrunkBlurred, gbuffer);

	// calc near coc
	nearCoC.begin();
	ofClear(0);
	{
		calcNearCoc.begin();
		calcNearCoc.setUniformTexture("shrunk", shrunk.getTexture(0), 1);
		calcNearCoc.setUniformTexture("shrunkBlurred", shrunkBlurred.getTexture(0), 2);
		nearCoC.draw(0, 0);
		calcNearCoc.end();

	}
	nearCoC.end();

	// blur color sample
	applySmallBlur(nearCoC.getTexture(), colorBlurred);

	if (isActiveBokeh) calcBokeh(gbuffer.getTexture(GBuffer::TYPE_ALBEDO));

	// output
	write.begin();
	ofClear(0);
	{
		ofEnableBlendMode(OF_BLENDMODE_ADD);

		applyDof.begin();
		applyDof.setUniformTexture("midBlur", colorBlurred.getTexture(), 1);
		applyDof.setUniformTexture("largeBlur", shrunkBlurred.getTexture(), 2);
		applyDof.setUniformTexture("depthTex", depth.getTexture(), 3);
		applyDof.setUniform1f("nearEndCoc", endPointsCoC->x);
		applyDof.setUniform1f("farEndCoc", endPointsCoC->y);
		applyDof.setUniform1f("foculRangeStart", foculRange->x);
		applyDof.setUniform1f("foculRangeEnd", foculRange->y);
		read.draw(0, 0);
		applyDof.end();

		if (isActiveBokeh) renderBokeh();

		ofDisableBlendMode();
	}
	write.end();

}

void DofPass::debugDraw(const glm::vec2& p, const glm::vec2& size) {
	
	glm::vec2 pos = p;

	ofDisableAlphaBlending();
	shrunkBlurred.draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	debugShader.begin();
	shrunkBlurred.draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	debugShader.end();

	colorBlurred.draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	depth.draw(pos, size.x, size.y);

	ofEnableAlphaBlending();
}

void DofPass::onTexParamChanged(int&) {

	bokehShapeTex.begin();
	ofClear(0);
	bokehShapingShader.begin();
	bokehShapingShader.setUniform1i("shapeId", bokehShapeId);
	bokehShapeTex.draw(0, 0);
	bokehShapingShader.end();
	bokehShapeTex.end();

}

void DofPass::onMaxBokehCountChanged(int&) {
	bokehColorTex.clear();
	bokehPosDepthCocTex.clear();

	bokehColorTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	bokehColorTex.allocate(maxBokehCount, 1, GL_RGBA32F);
	bokehPosDepthCocTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	bokehPosDepthCocTex.allocate(maxBokehCount, 1, GL_RGBA32F);

	atomicBuffer.setMaxCount(maxBokehCount);
}

void DofPass::applySmallBlur(const ofTexture& read, ofFbo& write) {
	write.begin();
	ofClear(0);
	smallBlur.begin();
	read.draw(0, 0);
	smallBlur.end();
	write.end();
}

void DofPass::calcBokeh(const ofTexture& read) {

	// Bind textures to store
	bokehColorTex.bindAsImage(0, GL_WRITE_ONLY);
	bokehPosDepthCocTex.bindAsImage(1, GL_WRITE_ONLY);
	atomicBuffer.bind();

	detectBokehShader.begin();
	detectBokehShader.setUniformTexture("tex", read, 0);
	detectBokehShader.setUniformTexture("depthTex", depth.getTexture(), 1);

	detectBokehShader.setUniform1f("cocThres", bokehCocThres.get());
	detectBokehShader.setUniform1f("lumThres", bokehLumThres.get());
	detectBokehShader.setUniform1f("farEndCoc", endPointsCoC->y);
	detectBokehShader.setUniform1f("foculRangeEnd", foculRange->y);
	detectBokehShader.dispatchCompute(read.getWidth(), read.getHeight(), 1);
	detectBokehShader.end();

	bokehColorTex.unbind();
	bokehPosDepthCocTex.unbind();
	atomicBuffer.unbind();
}

void DofPass::renderBokeh() {

	bokehRenderShader.begin();
	bokehRenderShader.setUniformTexture("bokehColor", bokehColorTex, 1);
	bokehRenderShader.setUniformTexture("bokehPosDepthCoc", bokehPosDepthCocTex, 2);
	bokehRenderShader.setUniformTexture("bokehTex", bokehShapeTex.getTexture(), 3);
	bokehRenderShader.setUniform1f("maxBokehRadius", maxBokehRadius);
	bokehRenderShader.setUniform1f("bokehDepthCutoff", bokehDepthCutoff);
	atomicBuffer.drawIndirect(vbo.getVbo(), GL_TRIANGLE_STRIP);
	bokehRenderShader.end();

}
