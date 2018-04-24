#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace ofxDeferredShading {
    class ShadowLightPass : public RenderPass, public ofNode {
    private:
        ofShader shader;
        ofFbo shadowMap;
        
        const glm::mat4 biasMat = ofMatrix4x4(0.5, 0.0, 0.0, 0.0,
                                                0.0, 0.5, 0.0, 0.0,
                                                0.0, 0.0, 0.5, 0.0,
                                                0.5, 0.5, 0.5, 1.0);
        
        glm::mat4 projection;
        glm::mat4 modelView;
        glm::mat4 depthMVP;
        glm::mat4 shadowTransMat;
        glm::vec3 posInViewSpace;
        
        float nearClip;
        float farClip;
        float distance;
        
        // view port
        float left = -1024.f;
        float right = 1024.f;
        float bottom = -1024.f;
        float top = 1024.f;
        
        ofFloatColor ambientColor;
        ofFloatColor diffuseColor;
        ofFloatColor specularColor;
        
    public:
        using Ptr = shared_ptr<ShadowLightPass>;
        ShadowLightPass(const ofVec2f& size);
        
        void beginShadowMap(bool bUseOwnShader = false);
        void endShadowMap();
        
        void debugDraw();
        
        void update(ofCamera& cam);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        
        void setFar(float _farClip) { farClip = _farClip; }
        void setNear(float _nearClip) { nearClip = _nearClip; }
        void setDirection(const glm::vec3& dir) {
            setGlobalPosition(dir * distance);
        }
        void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
        void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
        
    };
    
}   
