#pragma once

#include "ofMain.h"
#include "ofxDeferredShading.h"
#include "ofxGui.h"

#include "GenArchitect.hpp"
#include "GenCells.hpp"

using namespace ofxDeferred;

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
private:
    void setupDeferred();
    void updateDeferred();
    
    void setupGui();
    
    ofEasyCam cam;
    GenArchitecture archi;
    GenCells cells;
    
    ofxDeferredProcessing deferred;
    std::shared_ptr<PointLightPass> lightingPass;
	std::shared_ptr<SsaoPass> ssaoPass;
	std::shared_ptr<ShadowLightPass> shadowLightPass;
	std::shared_ptr<DofPass> dofPass;
	std::shared_ptr<HdrBloomPass> hdrPass;
    
    // gui
    bool isShowPanel;
    ofxPanel panel;
    ofParameterGroup pl1;
    ofParameter<glm::vec3> pl1_pos;
    ofParameter<ofFloatColor> pl1_diff;
    ofParameter<ofFloatColor> pl1_spe;
    ofParameter<float> pl1_int;
    ofParameter<float> pl1_rad;
    
    ofParameterGroup pl2;
    ofParameter<glm::vec3> pl2_pos;
    ofParameter<ofFloatColor> pl2_diff;
    ofParameter<ofFloatColor> pl2_spe;
    ofParameter<float> pl2_int;
    ofParameter<float> pl2_rad;
    
    ofParameterGroup ao;
    ofParameter<float> ao_rad;
    ofParameter<float> ao_dark;
    
    ofParameterGroup shadow;
    ofParameter<ofFloatColor> sha_amb;
    ofParameter<ofFloatColor> sha_dif;
    
    ofParameterGroup dof;
    ofParameter<float> dof_blur;
    ofParameter<float> dof_ape;
    ofParameter<float> dof_focal;
};

