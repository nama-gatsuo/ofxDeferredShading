#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace DeferredEffect {
    struct PointLight {
        ofFloatColor ambientColor;
        ofFloatColor diffuseColor;
        ofFloatColor specularColor;
        glm::vec3 position;
        float intensity = 1.f;
        float radius = 200.f;
    };
    
    class PointLightPass : public RenderPass {
    protected:
        vector<PointLight> lights;
        ofShader shader;
        glm::mat4 modelViewMatrix;
        ofVboMesh sphere;
        ofShader lightShader;
        
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
        
        void drawLights(ofPolyRenderMode mode = OF_MESH_FILL);
        void drawLights(ofCamera& cam, bool isShadow = false, ofPolyRenderMode mode = OF_MESH_FILL);
    };
}
