#pragma once

#include "ofMain.h"
#include "ofxDeferredShading.h"
#include "ofxGui.h"

#include "GenArchitect.hpp"
#include "GenCells.hpp"

using namespace ofxDeferred;

class ofApp : public ofBaseApp {
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
	ofPtr<PointLightPass> lightingPass;
	ofPtr<SsaoPass> ssaoPass;
	ofPtr<ShadowLightPass> shadowLightPass;
	ofPtr<DofPass> dofPass;
	ofPtr<BloomPass> bloomPass;

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
	ofParameter<float> sha_dark;

	ofParameterGroup dof;
	ofParameter<float> dof_focus;
	ofParameter<vec2> dof_coc;

	ofParameterGroup bloom;
	ofParameter<float> thres;
	
	ofParameter<float> fov;

};

