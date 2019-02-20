#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	archi.setup();
	cells.setup();

	setupDeferred();
	setupGui();

	cam.setFarClip(3000.);

}

//--------------------------------------------------------------
void ofApp::update() {
	shadowLightPass->setGlobalPosition(glm::normalize(glm::vec3(cos(ofGetElapsedTimef() * 0.5), 1.5f, sin(ofGetElapsedTimef() * 0.5))) * 1600.f);
	shadowLightPass->lookAt(glm::vec3(0));

	updateDeferred();
	
}

//--------------------------------------------------------------
void ofApp::draw() {

	shadowLightPass->beginShadowMap();
	archi.draw();
	cells.draw();
	lightingPass->drawLights();
	shadowLightPass->endShadowMap();

	deferred.begin(cam);
	archi.draw();
	cells.draw();
	lightingPass->drawLights();
	deferred.end(true);

	if (isShowPanel) {
		shadowLightPass->debugDraw();
		deferred.debugDraw();
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

	/*auto e = deferred.createPass<ofxDeferred::EdgePass>();
	e->setEdgeColor(1.);
	e->setUseReadColor(true);*/

	ssaoPass = deferred.createPass<ofxDeferred::SsaoPass>();

	shadowLightPass = deferred.createPass<ofxDeferred::ShadowLightPass>();
	shadowLightPass->setDarkness(0.9f);
	shadowLightPass->setViewPortSize(1440.f);
	shadowLightPass->setFar(2400.);
	shadowLightPass->setNear(400.);

	lightingPass = deferred.createPass<ofxDeferred::PointLightPass>();
	ofxDeferred::PointLight dlight;
	lightingPass->addLight(dlight);
	lightingPass->addLight(dlight);

	bloomPass = deferred.createPass<ofxDeferred::BloomPass>();
	dofPass = deferred.createPass<ofxDeferred::DofPass>();
}

void ofApp::updateDeferred() {
	auto& l1 = lightingPass->getLightRef(0);
	l1.position = pl1_pos.get();
	l1.diffuseColor = pl1_diff.get();
	l1.specularColor = pl1_spe.get();
	l1.radius = pl1_rad.get();
	l1.intensity = pl1_int.get();

	auto& l2 = lightingPass->getLightRef(1);
	l2.position = pl2_pos.get();
	l2.diffuseColor = pl2_diff.get();
	l2.specularColor = pl2_spe.get();
	l2.intensity = pl2_int.get();
	l2.radius = pl2_rad.get();

	ssaoPass->setOcculusionRadius(ao_rad.get());
	ssaoPass->setDarkness(ao_dark.get());

	shadowLightPass->setAmbientColor(sha_amb.get());
	shadowLightPass->setDiffuseColor(sha_dif.get());
	shadowLightPass->setDarkness(sha_dark.get());
	shadowLightPass->setNear(300.);

	bloomPass->setThreshold(thres.get());

	dofPass->setFoculRange(glm::vec2(dof_focus.get()));
	dofPass->setEndPointsCoC(dof_coc.get());
	dofPass->setMaxBlur(1.);
}

void ofApp::setupGui() {
	isShowPanel = false;

	panel.setup();
	pl1.setName("Point light 1");
	pl1.add(pl1_pos.set("Position", glm::vec3(500, 500, 500), glm::vec3(-1000), glm::vec3(1000)));
	pl1.add(pl1_diff.set("Diffuse Color", ofFloatColor(0.4)));
	pl1.add(pl1_spe.set("Specular Color", ofFloatColor(1.0)));
	pl1.add(pl1_rad.set("Radius", 500, 100, 2000));
	pl1.add(pl1_int.set("Intensity", 0.2, 0.1, 3.0));
	panel.add(pl1);

	pl2.setName("Point light 2");
	pl2.add(pl2_pos.set("Position", glm::vec3(-600, 700, 200), glm::vec3(-1000), glm::vec3(1000)));
	pl2.add(pl2_diff.set("Diffuse Color", ofFloatColor(0.4)));
	pl2.add(pl2_spe.set("Specular Color", ofFloatColor(1.0)));
	pl2.add(pl2_rad.set("Radius", 500, 100, 2000));
	pl2.add(pl2_int.set("Intensity", 0.5, 0.1, 3.0));
	panel.add(pl2);

	ao.setName("Ambient Occlusion");
	ao.add(ao_rad.set("Occlusion Radius", 5.0, 0.1, 100.0));
	ao.add(ao_dark.set("Darkness", 0.8, 0.1, 5.0));
	panel.add(ao);

	shadow.setName("Shadow Light");
	shadow.add(sha_amb.set("Ambient", ofFloatColor(0.2)));
	shadow.add(sha_dif.set("Diffuse", ofFloatColor(0.7)));
	shadow.add(sha_dark.set("Darkness", 0.5, 0., 1.));
	panel.add(shadow);

	bloom.setName("Bloom");
	bloom.add(thres.set("threshold", 0.5, 0., 0.99));
	panel.add(bloom);

	dof.setName("Defocus Blur");
	dof.add(dof_focus.set("Focus", 0.2, 0., 1.));
	dof.add(dof_coc.set("coc", glm::vec2(1., 0.5), glm::vec2(0.), glm::vec2(1.)));
	panel.add(dof);

	panel.minimizeAll();
	panel.loadFromFile("settings.xml");

}

void ofApp::keyPressed(int key) {
	if (key == 's') isShowPanel = !isShowPanel;
}
