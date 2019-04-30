#pragma once
#include "Processor.hpp"
#include "Blur.h"
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
		
		Blur blur;
		ofFbo blurred;
		ofFbo ssao;
	public:
		using Ptr = std::shared_ptr<SsaoPass>;
		SsaoPass(const glm::vec2& size);
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
		void update(const ofCamera& cam);
		void setOcculusionRadius(float radius) { this->radius = radius; }
		void setDarkness(float darkness) { this->darkness = darkness; }

	};
}
