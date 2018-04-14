#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace DeferredEffect {
    class ShadowLightPass : public RenderPass, public ofCamera {
    private:
        ofShader shader;
        
        ofFbo linearDepthMap;
        ofShader linearDepthShader;
        float linearDepthScalar;
        
        ofMatrix4x4 viewMat;
        ofMatrix4x4 projectionMat;
        const ofMatrix4x4 biasMat = ofMatrix4x4(0.5, 0.0, 0.0, 0.0,
                                                0.0, 0.5, 0.0, 0.0,
                                                0.0, 0.0, 0.5, 0.0,
                                                0.5, 0.5, 0.5, 1.0);
        ofMatrix4x4 shadowTransMat;
        ofVec3f posInViewSpace;
        
        bool bUseShader = false;
    public:
        typedef shared_ptr<ShadowLightPass> Ptr;
        ShadowLightPass(const ofVec2f& size);
        
        void beginShadowMap(bool bUseOwnShader = false);
        void endShadowMap();
        
        void debugDraw();
        
        void update(ofCamera& cam);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
    };
    
}   
