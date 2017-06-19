#include "ShadowLightPass.hpp"

using namespace DeferredEffect;

ShadowLightPass::ShadowLightPass(const ofVec2f& size) : RenderPass(size, "ShadowLightPass"){
    
    // create shadow map fbo
    ofVec2f res = ofVec2f(1024); // power of 2
    
    ofFbo::Settings s;
    s.width = res.x;  // TODO: need to controlable
    s.height = res.y; // TODO: need to controlable
    s.minFilter = GL_LINEAR;
    s.maxFilter = GL_LINEAR;
    s.wrapModeVertical = GL_CLAMP_TO_BORDER;
    s.wrapModeHorizontal = GL_CLAMP_TO_BORDER;
    s.internalformat = GL_R32F;
    s.useDepth = true;
    s.useStencil = true;
    s.depthStencilAsTexture = true;
    
    linearDepthMap.allocate(s);
    linearDepthMap.getTexture().getTextureData().bFlipTexture = true;
    
    // TODO: blur fbo
    
    // setup camera for shadow map
    // TODO: need to controlable
    float fov = 75.0, near = 0.1, far = 5000.0;
    
    lightCam.setupPerspective();
    lightCam.setAspectRatio(1.0);
    lightCam.setFov(fov);
    lightCam.setNearClip(near);
    lightCam.setFarClip(far);
    
    lightCam.setParent(*this);
    lightCam.setPosition(0, 0, 0);
    
    linearDepthScalar = 1.0 / (far - near);
    linearDepthShader.load("shader/vfx/linearDepth");
    linearDepthShader.begin();
    linearDepthShader.setUniform1f("linearDepthScalar", linearDepthScalar);
    linearDepthShader.end();
    
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
    
    projectionMat = lightCam.getProjectionMatrix();
    
    linearDepthMap.begin();
    
    ofEnableDepthTest();
    // if you want Exponential Shadow mappong, you need front face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    ofClear(0);
    lightCam.begin();
    
    if (!bUseShader) linearDepthShader.begin();
}

void ShadowLightPass::endShadowMap(){
    if (!bUseShader) linearDepthShader.end();
    
    lightCam.end();
    
    glDisable(GL_CULL_FACE);
    ofDisableDepthTest();
    
    linearDepthMap.end();
    
    // TODO: blur shadow map
    
}

void ShadowLightPass::setCam(float fov, float near, float far) {
    lightCam.setFov(fov);
    lightCam.setNearClip(near);
    lightCam.setFarClip(far);
    
    linearDepthScalar = 1.0 / (far - near);
    linearDepthShader.begin();
    linearDepthShader.setUniform1f("linearDepthScalar", linearDepthScalar);
    linearDepthShader.end();
}

void ShadowLightPass::debugDraw(){
    linearDepthMap.draw(0,0,256,256);
}

void ShadowLightPass::update(ofCamera &cam){
    
    ofMatrix4x4 invCamMat = ofMatrix4x4::getInverseOf(cam.getModelViewMatrix());
    shadowTransMat = invCamMat * viewMat * projectionMat * biasMat;
    
    posInViewSpace = getGlobalPosition() * cam.getModelViewMatrix();
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
    
    shader.setUniform4f("ambient", getAmbientColor());
    shader.setUniform4f("diffuse", getDiffuseColor());
    shader.setUniform1f("darkness", darkness);
    shader.setUniform1f("blend", blend);
    readFbo.draw(0,0);
    
    shader.end();
    
    writeFbo.end();
}
