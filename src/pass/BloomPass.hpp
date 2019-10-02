#pragma once
#include "RenderPass.hpp"
#include "BlurPass.h"

namespace ofxDeferred {
	class BloomPass : public RenderPass {
	private:
		PingPongBuffer blurred;
		std::vector<ofPtr<BlurPass>> blurs;
		std::vector<float> weights;

		ofShader lumaShader;
		ofShader composite;
		ofFbo lumaFbo;

		ofParameter<float> lumaThres;
		ofParameter<float> strength;
		const int numPass;
	public:
		using Ptr = std::shared_ptr<BloomPass>;
		
		BloomPass(const glm::vec2& size);
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		void setThreshold(float thres) { lumaThres = thres; }
		void debugDraw(const glm::vec2& p, const glm::vec2& size) override;
	};
}
