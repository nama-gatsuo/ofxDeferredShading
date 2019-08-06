#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	archi.setup();
	cells.setup();

	setupDeferred();
	
	cam.setFarClip(3000.);

}

//--------------------------------------------------------------
void ofApp::update() {
	shadowLightPass->setGlobalPosition(glm::normalize(glm::vec3(cos(ofGetElapsedTimef() * 0.5), 1.5f, sin(ofGetElapsedTimef() * 0.5))) * 1600.f);
	shadowLightPass->lookAt(glm::vec3(0));	
}

//--------------------------------------------------------------
void ofApp::draw() {

	shadowLightPass->beginShadowMap();
	archi.draw();
	cells.draw();
	pointLightPass->drawLights();
	shadowLightPass->endShadowMap();

	deferred.begin(cam);
	archi.draw();
	cells.draw();
	pointLightPass->drawLights();
	deferred.end();

	if (isShowPanel) {
		//deferred.debugDraw();
		//shadowLightPass->debugDraw();
		dof->debugDraw();
		//bloom->debugDraw();
		panel.draw();
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 12, 16);
	}

}

void ofApp::setupDeferred() {
	deferred.init(ofGetWidth(), ofGetHeight());

	auto bg = deferred.createPass<ofxDeferred::BgPass>();
	bg->begin();
	ofClear(1, 3, 6, 255);
	bg->end();

	auto e = deferred.createPass<ofxDeferred::EdgePass>();
	e->setEdgeColor(1.);
	e->setUseReadColor(true);

	deferred.createPass<ofxDeferred::SsaoPass>();

	shadowLightPass = deferred.createPass<ofxDeferred::ShadowLightPass>();
	shadowLightPass->setDarkness(0.9f);
	shadowLightPass->setViewPortSize(1440.f);
	shadowLightPass->setFar(3000.);
	shadowLightPass->setNear(400.);

	pointLightPass = deferred.createPass<ofxDeferred::PointLightPass>();
	pointLightPass->addLight();
	pointLightPass->addLight();
	
	dof = deferred.createPass<ofxDeferred::DofPass>();
	bloom = deferred.createPass<ofxDeferred::BloomPass>();
	
	panel.setup();
	panel.setDefaultBackgroundColor(ofFloatColor(0., 0.5));
	panel.setDefaultHeaderBackgroundColor(ofFloatColor(0.6, 0.6, 0.3, 0.6));
	panel.setDefaultFillColor(ofFloatColor(0.3, 0.3, 0.6, 0.6));
	panel.add(deferred.getParameters());
	panel.minimizeAll();
	panel.loadFromFile("settings.xml");
}

void ofApp::keyPressed(int key) {
	if (key == 's') isShowPanel = !isShowPanel;
	else if (key == 'l') pointLightPass->addLight();
}
