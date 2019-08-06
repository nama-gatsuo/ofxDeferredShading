#include "DofPass.hpp"

using namespace ofxDeferred;

DofPass::DofPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Dof), blur(size / 4., GL_RGBA) {

	shrunk.allocate(size.x / 4., size.y / 4., GL_RGBA);
	shrunkBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);
	nearCoC.allocate(size.x / 4., size.y / 4., GL_RGBA);
	colorBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);
	depth.allocate(size.x, size.y, GL_R32F);

	downSample.load(passThruPath, shaderPath + "dof/downSample.frag");
	calcNearCoc.load(passThruPath, shaderPath + "dof/calcNearCoc.frag");
	smallBlur.load(passThruPath, shaderPath + "dof/smallBlur.frag");
	applyDof.load(passThruPath, shaderPath + "dof/applyDof.frag");
	debugShader.load(passThruPath, shaderPath + "alphaFrag.frag");
	blur.setPreShrink(4);

	group.add(endPointsCoC.set("endpoint_coc", glm::vec2(0.9, 0.6), glm::vec2(0.), glm::vec2(1.)));
	group.add(foculRange.set("focul_range", glm::vec2(0.1, 0.3), glm::vec2(0.), glm::vec2(1.)));
	group.add(blur.getParameters());
	
	bokehGroup.setName("bokeh");
	bokehGroup.add(isActiveBokeh.set("isActiveBokeh", false));
	bokehGroup.add(maxBokehCount.set("maxBokehCount", 1000, 1, 1000));
	bokehGroup.add(bokehCocThres.set("bokehCocThres", 0.03, 0., 1.));
	bokehGroup.add(bokehLumThres.set("bokehLumThres", 0.03, 0., 1.));
	bokehGroup.add(maxBokehRadius.set("maxBokehRadius", 100., 0., 300.));
	bokehGroup.add(bokehDepthCutoff.set("bokehDepthCutoff", 0.03, 0.0, 1.0));
	group.add(bokehGroup);

	vbo = ofMesh::plane(1., 1.);

	bokehColorTex.allocate(maxBokehCount, 1, GL_RGBA32F);
	bokehPosDepthCocTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	bokehPosDepthCocTex.allocate(maxBokehCount, 1, GL_RGBA32F);
	
	bokehShapeTex.allocate(256, 256, GL_R8);

	detectBokehShader.setupShaderFromFile(GL_COMPUTE_SHADER, shaderPath + "bokeh/bokehCalc.glsl");
	detectBokehShader.linkProgram();
	bokehRenderShader.load(shaderPath + "bokeh/bokehRender");
	bokehShapingShader.load(passThruPath, shaderPath + "bokeh/bokehShaping.frag");

	bokehShapeTex.begin();
	ofClear(0);
	bokehShapingShader.begin();
	bokehShapeTex.draw(0, 0);
	bokehShapingShader.end();
	bokehShapeTex.end();
}

void DofPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {

	depth.begin();
	ofClear(0);
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
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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

void DofPass::debugDraw() {
	float w2 = ofGetViewportWidth();
	float h2 = ofGetViewportHeight();
	float ws = w2 * 0.25;
	float hs = h2 * 0.25;
	
	ofDisableAlphaBlending();
	shrunkBlurred.draw(0, hs * 3, ws, hs);
	debugShader.begin();
	shrunkBlurred.draw(ws * 1, hs * 3, ws, hs);
	debugShader.end();

	colorBlurred.draw(ws * 2, hs * 3, ws, hs);
	debugShader.begin();
	colorBlurred.draw(ws * 3, hs * 3, ws, hs);
	debugShader.end();
	
	if (isActiveBokeh) {
		bokehColorTex.draw(0, hs * 3., ws, 20.);
		bokehPosDepthCocTex.draw(0, hs * 3. + 20., ws, 20.);
		bokehShapeTex.draw(0, hs * 3. + 40., 256, 256);
	}

	ofEnableAlphaBlending();
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
	detectBokehShader.setUniformTexture("depth", depth.getTexture(), 1);

	detectBokehShader.setUniform1f("cocThres", bokehCocThres);
	detectBokehShader.setUniform1f("lumThres", bokehLumThres);
	detectBokehShader.setUniform1f("farEndCoc", endPointsCoC->y);
	detectBokehShader.setUniform1f("foculRangeEnd", foculRange->y);
	detectBokehShader.dispatchCompute(read.getWidth() / 4, read.getHeight() / 4, 1);
	detectBokehShader.end();
	
	bokehColorTex.unbind();
	bokehPosDepthCocTex.unbind();
	atomicBuffer.unbind();

	//ofLogNotice("atomic counter") << atomicBuffer.getCount();
}

void DofPass::renderBokeh() {
	int count = std::min(atomicBuffer.getCount(), maxBokehCount.get());
	if (count == 0) return;

	bokehRenderShader.begin();
	bokehRenderShader.setUniformTexture("bokehColor", bokehColorTex, 1);
	bokehRenderShader.setUniformTexture("bokehPosDepthCoc", bokehPosDepthCocTex, 2);
	bokehRenderShader.setUniformTexture("bokehTex", bokehShapeTex.getTexture(), 3);
	bokehRenderShader.setUniform1f("maxBokehRadius", maxBokehRadius);
	bokehRenderShader.setUniform1f("bokehDepthCutoff", bokehDepthCutoff);
	vbo.drawInstanced(OF_MESH_FILL, count);
	bokehRenderShader.end();

}
