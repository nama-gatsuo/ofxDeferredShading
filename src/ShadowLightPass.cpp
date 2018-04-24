#include "ShadowLightPass.hpp"

using namespace ofxDeferredShading;

ShadowLightPass::ShadowLightPass(const ofVec2f& size) : RenderPass(size, "ShadowLightPass"){
    
    // create shadow map fbo
    ofFbo::Settings s;
    s.width = 1024;
    s.height = 1024;
    s.minFilter = GL_LINEAR;
    s.maxFilter = GL_LINEAR;
    s.wrapModeVertical = GL_WRAP_BORDER;
    s.wrapModeHorizontal = GL_WRAP_BORDER;
    s.internalformat = GL_R32F;
    s.useDepth = true;
    s.useStencil = true;
    s.depthStencilAsTexture = true;
    
    shadowMap.allocate(s);
    
    // TODO: blur fbo
    // setup camera for shadow map
	nearClip = 0.f;
    farClip = 5000.f;
    distance = 3000.f;
    
    // load shader
    shader.load("shader/vfx/PassThru.vert", "shader/vfx/ShadowLight.frag");
    
}

void ShadowLightPass::beginShadowMap(bool bUseOwnShader){
    
    // update view matrix of depth camera
    projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
    modelView = glm::lookAt(getGlobalPosition(), glm::vec3(0,0,0), glm::vec3(0,1,0));
    depthMVP = projection * modelView;
    shadowMap.begin();
    
    ofClear(0);
    ofBackground(255,0,0);
    ofEnableDepthTest();
    
    ofPushView();
    ofSetMatrixMode(OF_MATRIX_PROJECTION);
    ofLoadMatrix(projection);
    ofSetMatrixMode(OF_MATRIX_MODELVIEW);
    ofLoadMatrix(modelView);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

}

void ShadowLightPass::endShadowMap(){
    
    glDisable(GL_CULL_FACE);
    ofDisableDepthTest();
    ofPopView();
    
    shadowMap.end();
    
    // TODO: blur shadow map
    
}

void ShadowLightPass::debugDraw(){
    shadowMap.getDepthTexture().draw(0,0, 128, 128);
}

void ShadowLightPass::update(ofCamera &cam){
    shadowTransMat = biasMat * depthMVP * glm::inverse(cam.getModelViewMatrix());
    posInViewSpace = (cam.getModelViewMatrix() * glm::vec4(getGlobalPosition(), 1.f)).xyz;
}

void ShadowLightPass::render(ofFbo &readFbo, ofFbo &writeFbo, GBuffer &gbuffer){
    
    writeFbo.begin();
    
    ofClear(0);
    
    shader.begin();
    shader.setUniformTexture("lightDepthTex", shadowMap.getDepthTexture(), 1);
    shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
    shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
    shader.setUniformMatrix4f("shadowTransMat", shadowTransMat);
    shader.setUniform3f("lightPosInViewSpace", posInViewSpace);
    shader.setUniform1f("darkness", darkness);
    
    shader.setUniform4f("ambient", ambientColor);
    shader.setUniform4f("diffuse", diffuseColor);
    readFbo.draw(0,0);
    
    shader.end();
    
    writeFbo.end();
}
