#pragma once
#include "ofMain.h"
#include "GBuffer.hpp"

namespace ofxDeferredShading {
    
    class RenderPass {
    public:
        typedef shared_ptr<RenderPass> Ptr;
        
        RenderPass(const ofVec3f& sz, const string& n) : size(sz), name(n), enabled(true) {}
        virtual void update(ofCamera& cam) = 0;
        virtual void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) = 0;
        
        void setEnabled(bool enabled) { this->enabled = enabled; }
        bool getEnabled() const { return enabled; }
    
    protected:
        void textureQuad(float x, float y, float w, float h, float s = 1.0, float t = 1.0);
        
        string name;
        bool enabled;
        ofVec2f size;
    };
    
    class Processor : public ofBaseDraws {
    public:
        using Ptr = std::shared_ptr<Processor>;
        void init(unsigned w = ofGetWidth(), unsigned h = ofGetHeight());
        
        void begin(ofCamera& cam, bool bUseOwnShader = false);
        void end(bool autoDraw = true);
        
        void draw(float x = 0., float y = 0.) const;
        void draw(float x, float y, float w, float h) const;
        float getWidth() const { return width; }
        float getHeight() const { return height; }
        
        void debugDraw();
        
        template<class T>
        shared_ptr<T> createPass(){
            shared_ptr<T> pass = shared_ptr<T>(new T(ofVec2f(width, height)));
            passes.push_back(pass);
            return pass;
        }
        
        unsigned size() const { return passes.size(); }
        RenderPass::Ptr operator[](unsigned i) const { return passes[i]; }
        vector<RenderPass::Ptr>& getPasses() { return passes; }
        unsigned getNumProcessedPasses() const { return numProcessedPasses; }
        
    private:
        void process();
        
        unsigned currentReadFbo;
        unsigned numProcessedPasses;
        unsigned width, height;
        
        GBuffer gbuffer;
        ofFbo pingPong[2];
        vector<RenderPass::Ptr> passes;
    };

}
