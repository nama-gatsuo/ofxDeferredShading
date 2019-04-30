#include "DofPass.hpp"

using namespace ofxDeferred;

DofPass::DofPass(const glm::vec2& size) : RenderPass(size, "DofPass"), blur(size / 4.) {

	shrunk.allocate(size.x / 4., size.y / 4., GL_RGBA);
	shrunkBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);
	nearCoC.allocate(size.x / 4., size.y / 4., GL_RGBA);
	colorBlurred.allocate(size.x / 4., size.y / 4., GL_RGBA);

	blur.setup(6, 1.f);

	downSample.load(passThruPath, shaderPath + "dof/downSample.frag");
	calcNearCoc.load(passThruPath, shaderPath + "dof/calcNearCoc.frag");
	smallBlur.load(passThruPath, shaderPath + "dof/smallBlur.frag");
	applyDof.load(passThruPath, shaderPath + "dof/applyDof.frag");

	group.add(endPointsCoC.set("endpoint_coc", glm::vec2(0.9, 0.6), glm::vec2(0.), glm::vec2(1.)));
	group.add(foculRange.set("focul_range", glm::vec2(0.1, 0.3), glm::vec2(0.), glm::vec2(1.)));

}

void DofPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {

	// downSample and initialize CoC
	shrunk.begin();
	ofClear(0);
	{
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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
	blur.blur(shrunk, shrunkBlurred);

	// calc near coc
	nearCoC.begin();
	ofClear(0);
	{
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		calcNearCoc.begin();
		calcNearCoc.setUniformTexture("shrunk", shrunk.getTexture(0), 1);
		calcNearCoc.setUniformTexture("shrunkBlurred", shrunk.getTexture(0), 2);
		nearCoC.draw(0, 0);
		calcNearCoc.end();

	}
	nearCoC.end();

	// blur color sample
	colorBlurred.begin();
	ofClear(0);
	{
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		smallBlur.begin();
		smallBlur.setUniformTexture("nearCoc", nearCoC.getTexture(), 1);
		colorBlurred.draw(0, 0);
		smallBlur.end();
	}
	colorBlurred.end();

	// output
	writeFbo.begin();
	ofClear(0);
	{
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		applyDof.begin();
		applyDof.setUniformTexture("midBlur", colorBlurred.getTexture(), 1);
		applyDof.setUniformTexture("largeBlur", shrunkBlurred.getTexture(), 2);
		applyDof.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
		applyDof.setUniform1f("nearEndCoc", endPointsCoC.get().x);
		applyDof.setUniform1f("foculRangeStart", foculRange.get().x);
		applyDof.setUniform1f("foculRangeEnd", foculRange.get().y);
		applyDof.setUniform1f("farEndCoc", endPointsCoC.get().y);
		readFbo.draw(0, 0);
		applyDof.end();
	}
	writeFbo.end();

}
