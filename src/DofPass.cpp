#include "DofPass.hpp"

using namespace DeferredEffect;

DofPass::DofPass(const ofVec2f& size) : RenderPass(size, "DofPass") {
    shader.load("shader/vfx/PassThru.vert", "shader/vfx/Dof.frag");
}

void DofPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer){
    writeFbo.begin();
    
    shader.begin();
    
    shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 1);
    shader.setUniform1f("aperture", aperture);
    shader.setUniform1f("focusDist", focus);
    shader.setUniform1f("maxBlur", maxBlur);
    shader.setUniform2f("size", size);
    readFbo.draw(0,0);
    
    shader.end();
    writeFbo.end();
}
