#include "BloomPass.hpp"

using namespace ofxDeferred;

BloomPass::BloomPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Bloom), blur(size, GL_RGB16F) {
	blurred.allocate(size.x, size.y, GL_RGB);
	
	lumaShader.load(passThruPath, shaderPath + "lumaThres.frag");
	lumaFbo.allocate(size.x, size.y, GL_RGB16F);
	composite.load(passThruPath, shaderPath + "bloom.frag");
	
	group.add(lumaThres.set("luma_threshold", 0.5, 0., 3.));
	group.add(strength.set("strength", 1., 0., 10.));
	group.add(blur.getParameters());
}


void BloomPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {
	
	ofDisableAlphaBlending();
	lumaFbo.begin();
	ofClear(0);
	{
		lumaShader.begin();
		lumaShader.setUniform1f("lumaThres", lumaThres);
		lumaShader.setUniformTexture("read", readFbo.getTexture(), 1);
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
		composite.begin();
		composite.setUniformTexture("blurredThres", blurred.getTexture(), 1);
		composite.setUniform1f("strength", strength);
		readFbo.draw(0, 0);
		composite.end();
		//ofSetColor(255, 128);
		//lumaFbo.draw(0, 0);
		//ofSetColor(255);
		//blurred.draw(0, 0);
		
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
	blurred.draw(ws, hs * 3, ws, hs);
	ofEnableAlphaBlending();
}