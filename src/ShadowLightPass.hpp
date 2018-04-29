#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace ofxDeferred {
    class ShadowLightPass : public RenderPass, public ofNode {
    private:
		ofShader shader;
		ofShader linearDepthShader;
		bool useShader = false;
		ofFbo shadowMap;
        
		const glm::mat4 biasMat = glm::mat4(0.5, 0.0, 0.0, 0.0,
											0.0, 0.5, 0.0, 0.0,
											0.0, 0.0, 0.5, 0.0,
											0.5, 0.5, 0.5, 1.0);
        
		glm::mat4 projection;
		glm::mat4 modelView;
		glm::mat4 depthMVP;
		glm::mat4 shadowTransMat;
		glm::vec3 directionInView;
		float linearDepthScalar;
        
		float nearClip;
		float farClip;
		float distance;
		glm::vec3 direction;
		ofCamera lightCamera;
        
		// view port
		float left = -1024.f;
		float right = 1024.f;
		float bottom = -1024.f;
		float top = 1024.f;
        
		float darkness = 0.f;
		ofFloatColor ambientColor;
		ofFloatColor diffuseColor;
		ofFloatColor specularColor;
        
    public:
        using Ptr = shared_ptr<ShadowLightPass>;
        ShadowLightPass(const ofVec2f& size);
        
        void beginShadowMap(bool bUseOwnShader = false);
        void endShadowMap();
        
        void debugDraw();
        ofFbo& getShadowMap() { return shadowMap; };
        
        void update(ofCamera& cam);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        
        void setDarkness(float _darkness) { darkness = _darkness; }
        void setFar(float _farClip) { farClip = _farClip; }
        void setNear(float _nearClip) { nearClip = _nearClip; }
        void setDirection(const glm::vec3& dir) {
			setGlobalPosition(- dir * distance);
			direction = dir;
        }
		ofCamera& getCamera() { return lightCamera; }
		float getLinearScalar() { return linearDepthScalar; }
        void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
        void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
        
    };
    
}   
