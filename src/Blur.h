#pragma once
#include "ofFbo.h"
#include "ofShader.h"
#include "ofMath.h"
#include "ofGraphics.h"

namespace ofxDeferred {

	// class to apply blur to fbo or texture
	class Blur {
	public:
		Blur(const glm::vec2& size) : size(size) {
			
		}
		~Blur() {}

		void setup(int blurRes, float sampleStep,  int preShrink = 2) {
			
			coefficients.clear();
			coefficients = createGaussianWeights(blurRes, 0.2f);

			this->preShrink = preShrink;
			buffer[0].allocate(size.x / (float)preShrink, size.y / (float)preShrink, GL_RGBA16F);
			buffer[1].allocate(size.x / (float)preShrink, size.y / (float)preShrink, GL_RGBA16F);

			shrinkShader.load("shader/vfx/PassThru.vert", "shader/vfx/Shrink.frag");

			blurShader.load("shader/vfx/PassThru.vert", "shader/vfx/Blur.frag");
			blurShader.begin();
			blurShader.setUniform1i("blurRes", blurRes);
			blurShader.setUniform1f("sampleStep", sampleStep);
			blurShader.setUniform1fv("coefficients", coefficients.data(), coefficients.size());
			blurShader.setUniform1i("preShrink", preShrink);
			blurShader.end();
		
		}

		void blur(const ofTexture& read, ofFbo& write) {

			glm::vec2 shrunkSize(size.x / (float)preShrink, size.y / (float)preShrink);

			buffer[0].begin();
			ofClear(0);
			//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			read.draw(0, 0, shrunkSize.x, shrunkSize.y);
			buffer[0].end();
			
			buffer[1].begin();
			ofClear(0);
			{
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				blurShader.begin();
				blurShader.setUniform1i("isHorizontal", 0);
				buffer[0].draw(0, 0);
				blurShader.end();
			}
			buffer[1].end();

			buffer[0].begin();
			ofClear(0);
			{
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				blurShader.begin();
				blurShader.setUniform1i("isHorizontal", 1);
				buffer[1].draw(0, 0, shrunkSize.x, shrunkSize.y);
				blurShader.end();
			}
			buffer[0].end();
			
			write.begin();
			ofClear(0);

			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			buffer[0].draw(0, 0, size.x, size.y);
			write.end();
		}

		void blur(const ofFbo& read, ofFbo& write) {
			blur(read.getTexture(), write);
		}

	private:

		static std::vector<float> createGaussianWeights(int radius, float variance) {

			int rowSize = 1 + radius;
			float sum = 0.f;
			std::vector<float> row;
			
			for (int i = 0; i < rowSize; i++) {
				
				float x = ofMap(i, 0, radius, 0, 1.2f);
				float w = Gaussian(x, 0, variance);

				row.push_back(w);

				if (i != 0) w *= 2.f;
				sum += w;
			}

			for (int i = 0; i < row.size(); i++) {
				row[i] /= sum;
			}

			return row;
		}

		static float Gaussian(float x, float mean, float variance) {
			x -= mean;
			return (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2. * variance));
		}

		std::vector<float> coefficients;
		vector<float> offsets;
		int preShrink;

		ofShader blurShader;
		ofShader shrinkShader;
		ofFbo buffer[2];

		glm::vec2 size;

	};
	


};
