#include "PointLightPass.hpp"

using namespace DeferredEffect;

PointLightPass::PointLightPass(const ofVec2f& size) : RenderPass(size, "PointLightPass"){
    shader.load("shader/vfx/PassThru.vert", "shader/vfx/PointLight.frag");
    lightShader.load("shader/gbuffer.vert", "shader/customShader.frag");
    
    sphere = ofMesh::sphere(50);
    sphere.clearColors();
    for (int i = 0; i < sphere.getNumNormals(); i++) {
        ofVec3f n = sphere.getNormal(i);
        sphere.addColor(ofFloatColor(1.05));
    }
}

void PointLightPass::update(ofCamera &cam){
    modelViewMatrix = cam.getModelViewMatrix();
}

void PointLightPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer){
    
    shader.begin();
    shader.setUniform1f("lAttenuation", 1.0);
    shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 1);
    shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 2);
    shader.setUniformTexture("readFbo", readFbo.getTexture(0), 4);
    shader.end();
    
    writeFbo.begin();
    ofClear(0);
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    readFbo.draw(0, 0);
    
    shader.begin();
    for (PointLight& light : lights) {
        
        ofVec3f lightPosInViewSpace = light.position * modelViewMatrix;
        shader.setUniform3f("lPosition", lightPosInViewSpace);
        shader.setUniform4f("lAmbient", light.ambientColor);
        shader.setUniform4f("lDiffuse", light.diffuseColor);
        shader.setUniform4f("lSpecular", light.specularColor);
        shader.setUniform1f("lIntensity", light.intensity);
        shader.setUniform1f("lRadius", light.radius);
        readFbo.draw(0,0);
    }
    
    shader.end();
    
    ofDisableBlendMode();
    ofPopStyle();
    writeFbo.end();
}

void PointLightPass::drawLights(){
    
    for (auto light : lights) {
        ofPushMatrix();
        ofTranslate(light.position);
        ofScale(light.intensity, light.intensity, light.intensity);
    
        sphere.draw();

        ofPopMatrix();
    }
    
}

void PointLightPass::drawLights(ofCamera& cam, bool isShadow){
   
    ofMatrix4x4 normalMatrix = ofMatrix4x4::getTransposedOf(cam.getModelViewMatrix().getInverse());
        
    lightShader.begin();
    lightShader.setUniform1i("isShadow", isShadow?1:0);
    lightShader.setUniformMatrix4f("normalMatrix", normalMatrix);
    lightShader.setUniform1f("farClip", cam.getFarClip());
    lightShader.setUniform1f("nearClip", cam.getNearClip());
        
    drawLights();
    
    lightShader.end();

}
