#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace DeferredEffect {
    class SsaoPass : public RenderPass {
    protected:
        ofShader shader;
        glm::mat4 projection;
        float radius = 2.0;
        float darkness = 0.8;
    public:
        typedef shared_ptr<SsaoPass> Ptr;
        SsaoPass(const ofVec2f& size);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        void update(ofCamera& cam);
        void setOcculusionRadius(float radius){ this->radius = radius; }
        void setDarkness(float darkness){ this->darkness = darkness; }
        
    };
}
