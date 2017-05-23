#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace DeferredEffect {
    class HdrBloomPass : public RenderPass {
    protected:
        ofShader blurShader;
        ofFbo blurFbo[2];
    public:
        typedef shared_ptr<HdrBloomPass> Ptr;
        HdrBloomPass(const ofVec2f& size);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        void update(ofCamera& cam);
        
    };
}
