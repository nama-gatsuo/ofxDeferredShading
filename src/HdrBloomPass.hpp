#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace ofxDeferredShading {
    class HdrBloomPass : public RenderPass {
    protected:
		int blurRadius;
		float blurVariance;
        ofShader blurShader;
		ofShader thresShader;
        ofFbo blurFbo[2];
		vector<float> coefficients;
		vector<float> offsets;

		vector<float> createGaussianWeights(int size, float variance);
		vector<float> createOffsets(vector<float>& gaussian);
		float Gaussian(float x, float mean, float variance);
    public:
        using Ptr = shared_ptr<HdrBloomPass>;
        HdrBloomPass(const ofVec2f& size);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        void update(ofCamera& cam);
        
    };
}
