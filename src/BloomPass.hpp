#pragma once
#include "DeferredProcessor.hpp"
#include "BlurPass.h"

namespace ofxDeferred {
	class BloomPass : public RenderPass {
	private:
		ofFbo blurred;
		BlurPass blur;
		
		ofShader lumaShader;
		ofFbo lumaFbo;

		ofParameter<float> lumaThres;

	public:
		using Ptr = std::shared_ptr<BloomPass>;
		
		BloomPass(const glm::vec2& size);
		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void setThreshold(float thres) { lumaThres = thres; }
		void debugDraw();
	};
}
