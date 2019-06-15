#pragma once
#include "DeferredProcessor.hpp"
#include "ofTypes.h"
namespace ofxDeferred {

	class PingPongBuffer {
	public:
		PingPongBuffer(const glm::vec2& res = glm::vec2(ofGetWidth(), ofGetHeight()), GLint intrenalFormat = GL_RGB) :
			res(res), currentSrc(0) {

			ofFboSettings s;
			s.width = res.x;
			s.height = res.y;
			s.internalformat = intrenalFormat;

			for (auto& b : buffers) {
				b = std::make_shared<ofFbo>();
				b->allocate(s);
			}

			src = buffers[currentSrc];
			dst = buffers[1 - currentSrc];
		}

		PingPongBuffer(const ofFboSettings& s) : res(s.width, s.height), currentSrc(0) {
			for (auto& b : buffers) {
				b = std::make_shared<ofFbo>();
				b->allocate(s);
			}

			src = buffers[currentSrc];
			dst = buffers[1 - currentSrc];
		}

		~PingPongBuffer() {}
		void resize(int w, int h) {
			res.x = w;
			res.y = h;

			for (auto b : buffers) {
				GLint intrenalFormat = b->getTexture().getTextureData().glInternalFormat;
				b->clear();

				ofFboSettings s;
				s.width = res.x;
				s.height = res.y;
				s.internalformat = intrenalFormat;

				b->allocate(s);
			}

		}
		ofPtr<ofFbo> operator[](int i) {
			return buffers[i];
		}
		void swap() {
			currentSrc = 1 - currentSrc;
			src = buffers[currentSrc];
			dst = buffers[1 - currentSrc];
		}

		ofPtr<ofFbo> src;
		ofPtr<ofFbo> dst;
	private:
		std::array<ofPtr<ofFbo>, 2> buffers;
		int currentSrc;
		glm::vec2 res;
	};
	
	class BlurPass : public RenderPass {
	public:
		using Ptr = ofPtr<BlurPass>;

		BlurPass(const glm::vec2& res, GLint colorFormat = GL_RGB);
		void render(ofFbo& read, ofFbo& write, GBuffer& gbuffer) override;
		void update(const ofCamera& cam) override {}
		//void resize(int w, int h) override;

		void onPreShrinkChanged(int& _value);
		void onBlurResChanged(int& _value);
		void onSampleStepChanged(float& _value);

		void setSampleStep(float v) { sampleStep.set(v); }
		void setBlurRes(int v) { blurRes.set(v); }
		void setPreShrink(int v) { preShrink.set(v); }

	private:
		inline std::vector<float> createGaussianWeights(int radius, float variance);
		inline float Gaussian(float x, float mean, float variance);

		std::vector<float> coefficients;

		ofParameter<int> preShrink;
		ofParameter<int> blurRes;
		ofParameter<float> sampleStep;

		ofShader shader;
		ofShader shrinkShader;
		PingPongBuffer pp;

	};

};
