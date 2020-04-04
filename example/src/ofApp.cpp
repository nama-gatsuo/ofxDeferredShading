#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	archi.setup();
	cells.setup();

	setupDeferred();
	panel.add(farClip.set("farClip", 3000., 10., 10000.));
	panel.add(isShowGbuffer.set("show_gbuffer", true));
	panel.add(isShowShadowMap.set("show_shadowMap", true));
	panel.add(isShowDofBuffer.set("show_dofBuffers", false));
	panel.add(isShowBloomBuffer.set("show_bloomBuffers", false));
}

//--------------------------------------------------------------
void ofApp::update() {
	cam.setFarClip(farClip);
	shadowLightPass->setGlobalPosition(glm::normalize(glm::vec3(cos(ofGetElapsedTimef() * 0.5), 0.4f + 0.2f * sin(ofGetElapsedTimef() * 0.5f) , sin(ofGetElapsedTimef() * 0.5))) * 1600.f);
	shadowLightPass->lookAt(glm::vec3(0));	
}

//--------------------------------------------------------------
void ofApp::draw() {

	shadowLightPass->beginShadowMap(cam);
	archi.draw();
	cells.draw();
	pointLightPass->drawLights();
	shadowLightPass->endShadowMap();

	deferred.begin(cam);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	archi.draw();
	cells.draw();
	pointLightPass->drawLights();
	glDisable(GL_CULL_FACE);
	
	deferred.end();

	if (isShowPanel) {
		if (isShowGbuffer) deferred.debugDraw();
		if (isShowShadowMap) shadowLightPass->debugDraw(glm::vec2(ofGetWidth()-512, 0), glm::vec2(512));
		if (isShowDofBuffer) dof->debugDraw(glm::vec2(0), glm::vec2(ofGetWidth(), ofGetHeight()) * 0.25f);
		if (isShowBloomBuffer) bloom->debugDraw(glm::vec2(0, ofGetHeight() * 0.25), glm::vec2(ofGetWidth(), ofGetHeight()) * 0.25f);
		panel.draw();
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 12, 16);
	}

}

void ofApp::setupDeferred() {
	deferred.init(ofGetWidth(), ofGetHeight());
	
	auto bg = deferred.createPass<ofxDeferred::BgPass>();

	auto e = deferred.createPass<ofxDeferred::EdgePass>();
	deferred.createPass<ofxDeferred::SsaoPass>();
	shadowLightPass = deferred.createPass<ofxDeferred::ShadowLightPass>();
	pointLightPass = deferred.createPass<ofxDeferred::PointLightPass>();
	pointLightPass->addLight();
	pointLightPass->addLight();
	deferred.createPass<ofxDeferred::FxaaPass>();
	
	deferred.createPass<ofxDeferred::FogPass>();
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
