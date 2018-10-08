#include "BloomPass.hpp"

using namespace ofxDeferred;

BloomPass::BloomPass(const glm::vec2& size) : RenderPass(size, "BloomPass"), blur(size) {
	blurred.allocate(size.x, size.y, GL_RGBA16F);
	blur.setup(12, 1.);

	lumaShader.load("shader/vfx/PassThru.vert", "shader/vfx/LumaThres.frag");
	lumaFbo.allocate(size.x, size.y, GL_RGBA16F);

	lumaThres = 0.5;
}


void BloomPass::render(ofFbo& readFbo, ofFbo &writeFbo, GBuffer &gbuffer) {
	
	lumaFbo.begin();
	ofClear(0);
	{
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		lumaShader.begin();
		lumaShader.setUniform1f("lumaThres", lumaThres);
		gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
		lumaShader.end();
	}
	lumaFbo.end();

	blur.blur(lumaFbo, blurred);	

	writeFbo.begin();
	ofClear(0);
	{
		ofPushStyle();
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		readFbo.draw(0, 0);
		blurred.draw(0, 0);
		
		ofDisableBlendMode();
		ofDisableAlphaBlending();
		ofPopStyle();
	}
	writeFbo.end();

}
