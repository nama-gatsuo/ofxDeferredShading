#include "ShadowLightPass.hpp"

using namespace DeferredEffect;

ShadowLightPass::ShadowLightPass(const ofVec2f& size) : RenderPass(size, "ShadowLightPass"){
    
    // create shadow map fbo
    ofVec2f res = ofVec2f(1024); // power of 2
    
    ofFbo::Settings s;
    s.width = res.x;
    s.height = res.y;
    s.minFilter = GL_LINEAR;
    s.maxFilter = GL_LINEAR;
    s.wrapModeVertical = GL_WRAP_BORDER;
    s.wrapModeHorizontal = GL_WRAP_BORDER;
    s.internalformat = GL_R32F;
    s.useDepth = true;
    s.useStencil = true;
    s.depthStencilAsTexture = true;
    
    linearDepthMap.allocate(s);
    linearDepthMap.getTexture().getTextureData().bFlipTexture = true;
    
    // TODO: blur fbo
    // setup camera for shadow map
	float fov_ = 75.0;
	float near_ = 0.1;
	float far_ = 1500.0;
    
    setupPerspective();
    setAspectRatio(1.0);
    
    linearDepthShader.load("shader/vfx/linearDepth");
    setPosition(0, 800, 700);
    setFov(fov_);
    setNearClip(near_);
    setFarClip(far_);
    linearDepthScalar = 1.0 / (far_ - near_);
    // load shader
    shader.load("shader/vfx/PassThru.vert", "shader/vfx/ShadowLight.frag");
    shader.begin();
    shader.setUniform1f("linearDepthScalar", linearDepthScalar);
    shader.end();
    
}

void ShadowLightPass::beginShadowMap(bool bUseOwnShader){
    
    bUseShader = bUseOwnShader;
    
    // update view matrix of depth camera
    ofVec3f eye = getGlobalPosition();
    ofVec3f center = eye + getLookAtDir();
    ofVec3f up = getUpDir();
    viewMat.makeLookAtViewMatrix(eye, center, up);
    
    projectionMat = getProjectionMatrix();
    
    linearDepthMap.begin();
    ofClear(0);
    
    begin();
    ofBackground(255,0,0);
    
    
    ofEnableDepthTest();
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    if (!bUseShader) {
        linearDepthShader.begin();
        linearDepthShader.setUniform1f("linearDepthScalar", linearDepthScalar);
    }

}

void ShadowLightPass::endShadowMap(){
    if (!bUseShader) linearDepthShader.end();
    
    glDisable(GL_CULL_FACE);
    ofDisableDepthTest();
    
    end();
    linearDepthMap.end();
    
    // TODO: blur shadow map
    
}

void ShadowLightPass::debugDraw(){
    linearDepthMap.draw(0,0, 128, 128);
}

void ShadowLightPass::update(ofCamera &cam){
    
    linearDepthScalar = 1.0 / (getFarClip() - getNearClip());
    
    ofMatrix4x4 invCamMat = ofMatrix4x4::getInverseOf(cam.getModelViewMatrix());
    shadowTransMat = invCamMat * viewMat * projectionMat * biasMat;
    
    posInViewSpace = cam.getModelViewMatrix() * glm::vec4(getGlobalPosition(), 1.f);
    
}

void ShadowLightPass::render(ofFbo &readFbo, ofFbo &writeFbo, DeferredEffect::GBuffer &gbuffer){
    
    writeFbo.begin();
    
    ofClear(0);
    
    shader.begin();
    shader.setUniformTexture("lightDepthTex", linearDepthMap.getTexture(), 1);
    shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
    shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
    shader.setUniformMatrix4f("shadowTransMat", shadowTransMat);
    shader.setUniform3f("lightPosInViewSpace", posInViewSpace);
    linearDepthShader.setUniform1f("linearDepthScalar", linearDepthScalar);
    
    shader.setUniform4f("ambient", ofFloatColor(0.2));
    shader.setUniform4f("diffuse", ofFloatColor(0.8));
    readFbo.draw(0,0);
    
    shader.end();
    
    writeFbo.end();
}
