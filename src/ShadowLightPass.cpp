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
    s.internalformat = GL_R16F;
    s.useDepth = true;
    s.useStencil = true;
    s.depthStencilAsTexture = true;
    
    linearDepthMap.allocate(s);
    linearDepthMap.getTexture().getTextureData().bFlipTexture = true;
    
    // TODO: blur fbo
    // setup camera for shadow map
    float fov = 75.0f, near = 0.1f, far = 1800.f;
    
    //setupPerspective();
    setAspectRatio(1.0);
    setFov(fov);
    setNearClip(near);
    setFarClip(far);
    setVFlip(true);
    setPosition(0, 800, 700);
    
    linearDepthScalar = 1.0 / (far - near);
    
    // load shader
    shader.load("shader/vfx/PassThru.vert", "shader/vfx/ShadowLight.frag");
    linearDepthShader.load("shader/vfx/linearDepth");
    
}

void ShadowLightPass::beginShadowMap(bool bUseOwnShader){
    
    bUseShader = bUseOwnShader;
    
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
    shadowTransMat = cam.getModelViewMatrix().getInverse() * getModelViewProjectionMatrix() * biasMat;
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
    shader.setUniform1f("linearDepthScalar", linearDepthScalar);
    
    shader.setUniform4f("ambient", ofFloatColor(0.2));
    shader.setUniform4f("diffuse", ofFloatColor(0.8));
    readFbo.draw(0,0);
    
    shader.end();
    
    writeFbo.end();
}
