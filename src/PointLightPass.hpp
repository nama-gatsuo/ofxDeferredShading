#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace DeferredEffect {
    struct PointLight {
        ofFloatColor ambientColor;
        ofFloatColor diffuseColor;
        ofFloatColor specularColor;
        ofVec3f position;
        float intensity = 1.0;
        float radius = 200.0;
    };
    
    class PointLightPass : public RenderPass {
    protected:
        vector<PointLight> lights;
        ofShader shader;
        ofMatrix4x4 projectionMatrix;
        ofMatrix4x4 modelViewMatrix;
        ofVboMesh sphere;
        
    public:
        typedef shared_ptr<PointLightPass> Ptr;
        PointLightPass(const ofVec2f& size);
        
        void addLight(PointLight light) {
            lights.push_back(light);
        }
        PointLight& getLightRef(int index) {
            return lights[index];
        }
        void clear(){ lights.clear(); }
        int getLightsSize() const { return lights.size(); }
        vector<PointLight>& getLights() { return lights; }
        void update(ofCamera& cam);
        void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
        
        void drawLights();
    };
}
