#pragma once
#include "ofMain.h"

namespace ofxDeferredShading {
    
    class GBuffer {
    private:
        ofFbo fbo;
        ofShader shader;
        ofShader debugShader;
        ofMatrix4x4 prevMvpMat;
        bool bUseShader;
    public:
        
        enum BufferType {
            TYPE_ALBEDO = 0, TYPE_POSITION = 1, TYPE_DEPTH_NORMAL = 2, TYPE_HDR = 3
        };
        GBuffer(){}
        void setup(int w = ofGetWidth(), int h = ofGetHeight());
        void begin(ofCamera& cam, bool bUseOthreShader = false);
        void end();
        void debugDraw();
        ofTexture& getTexture(int index){
            return fbo.getTexture(index);
        }
        ofFbo& getFbo(){
            return fbo;
        }
    };
}
