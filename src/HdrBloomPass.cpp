#include "HdrBloomPass.hpp"

using namespace ofxDeferredShading;

HdrBloomPass::HdrBloomPass(const ofVec2f& size) : RenderPass(size, "HdrBloomPass"){
    blurShader.load("shader/vfx/PassThru.vert", "shader/vfx/Blur.frag");
    blurFbo[0].allocate(size.x, size.y, GL_RGB);
    blurFbo[1].allocate(size.x, size.y, GL_RGB);
}

void HdrBloomPass::update(ofCamera& cam) {
    
}

void HdrBloomPass::render(ofFbo &readFbo, ofFbo &writeFbo, GBuffer &gbuffer) {
    
    // vertical blur
    blurFbo[0].begin();
    ofClear(0);
    blurShader.begin();
    blurShader.setUniform1i("horizontal", 0);
    gbuffer.getTexture(GBuffer::TYPE_HDR).draw(0,0);
    blurShader.end();
    blurFbo[0].end();
    
    // horizontal blur
    blurFbo[1].begin();
    ofClear(0);
    blurShader.begin();
    blurShader.setUniform1i("horizontal", 1);
    blurFbo[0].draw(0,0);
    blurShader.end();
    blurFbo[1].end();
    
    // write start
    writeFbo.begin();
    ofPushStyle();
    ofClear(0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    blurFbo[1].draw(0,0);
    readFbo.draw(0,0);
    
    ofDisableBlendMode();
    ofPopStyle();
    writeFbo.end();
    
}
