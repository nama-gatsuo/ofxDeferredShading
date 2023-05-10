#pragma once
#include "RenderPass.hpp"
#include "BlurPass.h"
#include "ofImage.h"
#include "ofPixels.h"

namespace ofxDeferred {
	class SsaoPass : public RenderPass {
	private:
		ofShader calcAo;
		ofShader applyAo;
		glm::mat4 projection;
		ofParameter<float> radius;
		ofParameter<float> darkness;
		ofFloatImage noiseTex;
		std::vector<glm::vec3> ssaoKernel;
		const int kernelSize;
		
		BlurPass blur;
		ofFbo blurred;
		ofFbo ssao;
	public:
		using Ptr = std::shared_ptr<SsaoPass>;
		SsaoPass(const glm::vec2& size);
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;
		void update(const ofCamera& cam);
		void refer(SsaoPass& pass);
		void setOcculusionRadius(float radius) { this->radius = radius; }
		void setDarkness(float darkness) { this->darkness = darkness; }
		void debugDraw(const glm::vec2& p, const glm::vec2& size) override;
	};
}
