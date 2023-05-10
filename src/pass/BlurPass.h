#pragma once
#include "RenderPass.hpp"
#include "PingPongBuffer.h"

namespace ofxDeferred {
	
	class BlurPass : public RenderPass {
	public:
		using Ptr = ofPtr<BlurPass>;

		BlurPass(const glm::vec2& res, GLint colorFormat = GL_RGB);
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;
		void render(const ofTexture& read, ofFbo& write);
		void update(const ofCamera& cam) override {}
		//void resize(int w, int h) override;

		void onPreShrinkChanged(int& _value);
		void onBlurResChanged(int& _value);
		void onSampleStepChanged(float& _value);

		void setSampleStep(float v) { sampleStep.set(v); }
		void setBlurRes(int v) { blurRes.set(v); }
		void setPreShrink(int v) { preShrink.set(v); }
		const ofTexture& getBlurred() const {
			return pp.src->getTexture();
		}

		static float Gaussian(float x, float mean, float variance) {
			x -= mean;
			return (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2. * variance));
		}
		void refer(BlurPass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			preShrink.makeReferenceTo(pass.preShrink);
			blurRes.makeReferenceTo(pass.blurRes);
			sampleStep.makeReferenceTo(pass.sampleStep);
		}

	private:
		inline std::vector<float> createGaussianWeights(int radius, float variance);

		std::vector<float> coefficients;

		ofParameter<int> preShrink;
		ofParameter<int> blurRes;
		ofParameter<float> sampleStep;

		ofShader shader;
		ofShader shrinkShader;
		PingPongBuffer pp;

	};

};
