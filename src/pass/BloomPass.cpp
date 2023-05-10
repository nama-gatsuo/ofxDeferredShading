#include "BloomPass.hpp"

using namespace ofxDeferred;

BloomPass::BloomPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Bloom), numPass(5), blurred(size) {
	
	composite.load(passThruPath, shaderPath + "bloom.frag");
	lumaShader.load(passThruPath, shaderPath + "lumaThres.frag");

	ofFboSettings s;
	s.width = size.x;
	s.height = size.y;
	s.internalformat = GL_RGBA8;
	s.minFilter = GL_NEAREST;
	s.maxFilter = GL_NEAREST;
	s.numSamples = 0;
	s.numColorbuffers = 1;
	s.useDepth = false;
	s.useStencil = false;

	lumaFbo.allocate(s);
	blurred.resize(s);

	for (int i = 0; i < numPass; i++) {
		float r = pow(2.f, (i + 1));
		glm::ivec2 res(size / r);
		
		ofPtr<BlurPass> blur = std::make_shared<BlurPass>(res, GL_RGBA8);
		blur->setBlurRes(6);
		blur->setSampleStep(1.0f);
		blur->setPreShrink(1);
		blurs.push_back(blur);

		//float g = 1. - 1. / r;
		float g = exp(-pow(3.f * r / size.x, 2.));
		weights.push_back(g);
	}

	group.add(lumaThres.set("luma_threshold", 0.5, 0., 3.));
	group.add(strength.set("strength", 1., 0., 10.));
	
}


void BloomPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {
	
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

	blurs[0]->render(lumaFbo.getTexture(), *(blurred.src), gbuffer);
	for (int i = 1; i < numPass; i++) {	
		blurs[i]->render(blurred.src->getTexture(), *(blurred.dst), gbuffer);
		blurred.swap();
	}

	write.begin();
	ofClear(0);
	{
		composite.begin();
		composite.setUniform1fv("weights", weights.data(), numPass);
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
	blurs[1]->getBlurred().draw(pos, size.x/2, size.y/2); pos += glm::vec2(size.x/2, 0);
	blurs[2]->getBlurred().draw(pos, size.x/4, size.y/4); pos += glm::vec2(size.x / 4, 0);
	blurs[3]->getBlurred().draw(pos, size.x / 8, size.y / 8); pos += glm::vec2(size.x / 8, 0);
	blurs[4]->getBlurred().draw(pos, size.x / 16, size.y / 16); pos += glm::vec2(size.x / 8, 0);
	ofEnableAlphaBlending();
}