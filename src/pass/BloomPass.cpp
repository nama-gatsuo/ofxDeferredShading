#include "BloomPass.hpp"

using namespace ofxDeferred;

BloomPass::BloomPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Bloom), numPass(8) {
	
	composite.load(passThruPath, shaderPath + "bloom.frag");
	lumaShader.load(passThruPath, shaderPath + "lumaThres.frag");

	ofFboSettings s;
	s.width = size.x;
	s.height = size.y;
	s.internalformat = GL_RGB16F;
	s.minFilter = GL_NEAREST;
	s.maxFilter = GL_NEAREST;
	s.numSamples = 1;
	s.numColorbuffers = 1;
	s.useDepth = false;
	s.useStencil = false;

	lumaFbo.allocate(s);
	blurred.allocate(s);

	for (int i = 0; i < numPass; i++) {
		
		glm::ivec2 res(size / pow(2., (i + 1)));
		ofLogNotice() << res;
		ofPtr<BlurPass> blur = std::make_shared<BlurPass>(res, GL_RGB16F);
		blur->setBlurRes(3);
		blur->setSampleStep(1.);
		blur->setPreShrink(1);
		blurs.push_back(blur);

		float g = 1. / pow(1.5, (i + 1));
		ofLogNotice() << g;
		weights.push_back(g);
	}

	group.add(lumaThres.set("luma_threshold", 0.5, 0., 3.));
	group.add(strength.set("strength", 1., 0., 10.));
	
	
}


void BloomPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {
	
	ofDisableAlphaBlending();
	lumaFbo.begin();
	ofClear(0);
	{
		lumaShader.begin();
		lumaShader.setUniform1f("lumaThres", lumaThres);
		lumaShader.setUniformTexture("read", read, 1);
		gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
		lumaShader.end();
	}
	lumaFbo.end();

	for (int i = 0; i < numPass; i++) {		
		blurs[i]->render(lumaFbo.getTexture(), blurred, gbuffer);
	}

	write.begin();
	ofClear(0);
	{
		
		composite.begin();
		//composite.setUniformTexture("blurredThres", blurred.getTexture(), 1);
		composite.setUniform1i("numPass", numPass);
		composite.setUniform1fv("weights", weights.data(), 8);
		for (int i = 0; i < numPass; i++) {
			composite.setUniformTexture("pass" + ofToString(i), blurs[i]->getBlurred(), i + 1);
		}
		composite.setUniform1f("strength", strength);
		read.draw(0, 0);
		composite.end();
		
		
		
	}
	write.end();

}

void BloomPass::debugDraw(const glm::vec2& p, const glm::vec2& size) {
	glm::vec2 pos(p);
	ofDisableAlphaBlending();
	lumaFbo.draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	blurs[0]->getBlurred().draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	blurs[1]->getBlurred().draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	blurs[2]->getBlurred().draw(pos, size.x, size.y);
	ofEnableAlphaBlending();
}