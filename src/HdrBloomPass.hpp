#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace ofxDeferred {
	class HdrBloomPass : public RenderPass {
	private:
		int blurRadius;
		float blurVariance;
		ofShader blurShader;
		
		ofFbo blurFbo[2];
		vector<float> coefficients;
		vector<float> offsets;

		vector<float> createGaussianWeights(int size, float variance);
		vector<float> createOffsets(const vector<float>& gaussian);
		float Gaussian(float x, float mean, float variance);
	public:
		using Ptr = shared_ptr<HdrBloomPass>;
		HdrBloomPass(const glm::vec2& size);
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
		void update(const ofCamera& cam) {}

	};
}
