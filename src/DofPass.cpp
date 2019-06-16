#include "DofPass.hpp"

using namespace ofxDeferred;

DofPass::DofPass(const glm::vec2& size) : RenderPass(size, "DofPass"), blur(size / 4., GL_RGBA) {

	shrunk.allocate(size.x / 4., size.y / 4., GL_RGBA);
	shrunkBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);
	nearCoC.allocate(size.x / 4., size.y / 4., GL_RGBA);
	colorBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);

	downSample.load(passThruPath, shaderPath + "dof/downSample.frag");
	calcNearCoc.load(passThruPath, shaderPath + "dof/calcNearCoc.frag");
	smallBlur.load(passThruPath, shaderPath + "dof/smallBlur.frag");
	applyDof.load(passThruPath, shaderPath + "dof/applyDof.frag");
	debugShader.load(passThruPath, shaderPath + "alphaFrag.frag");
	blur.setPreShrink(4);

	group.add(endPointsCoC.set("endpoint_coc", glm::vec2(0.9, 0.6), glm::vec2(0.), glm::vec2(1.)));
	group.add(foculRange.set("focul_range", glm::vec2(0.1, 0.3), glm::vec2(0.), glm::vec2(1.)));
	group.add(blur.getParameters());

}

void DofPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {

	ofDisableAlphaBlending();
	// downSample and initialize CoC
	shrunk.begin();
	ofClear(0);
	{
		downSample.begin();
		downSample.setUniformTexture("colorTex", readFbo.getTexture(), 1);
		downSample.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 2);
		downSample.setUniform1f("nearEndCoc", endPointsCoC.get().x);
		downSample.setUniform1f("foculRangeStart", foculRange.get().x);
		shrunk.draw(0, 0);
		downSample.end();
	}
	shrunk.end();

	// blur downSampled CoC image
	blur.render(shrunk, shrunkBlurred, gbuffer);

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

	// output
	writeFbo.begin();
	ofClear(0);
	{
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		applyDof.begin();
		applyDof.setUniformTexture("midBlur", colorBlurred.getTexture(), 1);
		applyDof.setUniformTexture("largeBlur", shrunkBlurred.getTexture(), 2);
		applyDof.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
		applyDof.setUniform1f("nearEndCoc", endPointsCoC.get().x);
		applyDof.setUniform1f("farEndCoc", endPointsCoC.get().y);
		applyDof.setUniform1f("foculRangeStart", foculRange.get().x);
		applyDof.setUniform1f("foculRangeEnd", foculRange.get().y);
		readFbo.draw(0, 0);
		applyDof.end();
	}
	writeFbo.end();

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
