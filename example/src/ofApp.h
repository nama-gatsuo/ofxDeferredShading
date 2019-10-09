#pragma once

#include "ofMain.h"
#include "ofxDeferredShading.h"
#include "ofxGui.h"

#include "GenArchitect.hpp"
#include "GenCells.hpp"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
private:
	void setupDeferred();

	ofEasyCam cam;
	GenArchitecture archi;
	GenCells cells;

	ofxDeferredProcessing deferred;
	ofxDeferred::PointLightPass::Ptr pointLightPass;
	ofxDeferred::ShadowLightPass::Ptr shadowLightPass;
	ofxDeferred::DofPass::Ptr dof;
	ofxDeferred::BloomPass::Ptr bloom;

	// gui
	bool isShowPanel;
	ofxPanel panel;
	ofParameter<float> farClip;
};
