#pragma once
#include "Processor.hpp"

namespace ofxDeferredShading {
    class DofPass : public RenderPass {
    private:
        ofShader shader;
        float focus;
        float aperture;
        float maxBlur;
    public:
        typedef shared_ptr<DofPass> Ptr;
        
        DofPass(const ofVec2f& size);
        void update(ofCamera& cam){};
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        
        void setFocus(float focus) { this->focus = focus; }
        void setAperture(float aperture) { this->aperture = aperture; }
        void setMaxBlur(float maxBlur) { this->maxBlur = maxBlur; }
    };
}
