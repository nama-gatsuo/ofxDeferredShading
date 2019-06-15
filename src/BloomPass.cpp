#include "BloomPass.hpp"

using namespace ofxDeferred;

BloomPass::BloomPass(const glm::vec2& size) : RenderPass(size, "BloomPass"), blur(size, GL_RGB32F) {
	blurred.allocate(size.x, size.y, GL_RGB);
	
	lumaShader.load(passThruPath, shaderPath + "lumaThres.frag");
	lumaFbo.allocate(size.x, size.y, GL_RGB32F);

	group.add(lumaThres.set("luma_threshold", 0.5, 0., 1.));
	group.add(blur.getParameters());
}


void BloomPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {
	
	ofDisableAlphaBlending();
	lumaFbo.begin();
	ofClear(0);
	{
		lumaShader.begin();
		lumaShader.setUniform1f("lumaThres", lumaThres);
		gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
		lumaShader.end();
	}
	lumaFbo.end();

	blur.render(lumaFbo, blurred, gbuffer);	

	writeFbo.begin();
	ofClear(0);
	{
		ofPushStyle();
		ofEnableAlphaBlending();
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		readFbo.draw(0, 0);
		blurred.draw(0, 0);
		
		ofDisableBlendMode();
		ofDisableAlphaBlending();
		ofPopStyle();
	}
	writeFbo.end();

}

void BloomPass::debugDraw() {
	float w2 = ofGetViewportWidth();
	float h2 = ofGetViewportHeight();
	float ws = w2 * 0.25;
	float hs = h2 * 0.25;

	ofDisableAlphaBlending();
	lumaFbo.draw(0, hs * 3, ws, hs);
	blurred.draw(ws * 2, hs * 3, ws, hs);
	ofEnableAlphaBlending();
}