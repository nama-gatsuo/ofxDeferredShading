#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	archi.setup();
	cells.setup();

	setupDeferred();
	setupGui();
	panel.add(fov.set("fov", 60, 30, 90));
	cam.setFarClip(3000);
	
}

//--------------------------------------------------------------
void ofApp::update() {
	//shadowLightPass->setGlobalPosition(normalize(vec3(cos(ofGetElapsedTimef() * 0.5), 1.5f, sin(ofGetElapsedTimef() * 0.5))) * 1600.f);
	shadowLightPass->setGlobalPosition(normalize(vec3(cos(0.), 1.5f, sin(0.))) * 1600.f);
	shadowLightPass->lookAt(vec3(0));

	updateDeferred();
	cam.setFov(fov.get());
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
	ssaoPass = deferred.createPass<SsaoPass>();

	shadowLightPass = deferred.createPass<ShadowLightPass>();
	shadowLightPass->setDarkness(0.9f);
	shadowLightPass->setViewPortSize(1440.f);

	lightingPass = deferred.createPass<PointLightPass>();
	ofxDeferred::PointLight dlight;
	dlight.ambientColor = ofFloatColor(0.f);
	lightingPass->addLight(dlight);

	dlight.ambientColor = ofFloatColor(0.f);
	lightingPass->addLight(dlight);

	bloomPass = deferred.createPass<BloomPass>();
	
	dofPass = deferred.createPass<DofPass>();
}

void ofApp::updateDeferred() {
	lightingPass->getLightRef(0).position = pl1_pos.get();
	lightingPass->getLightRef(0).diffuseColor = pl1_diff.get();
	lightingPass->getLightRef(0).specularColor = pl1_spe.get();
	lightingPass->getLightRef(0).radius = pl1_rad.get();
	lightingPass->getLightRef(0).intensity = pl1_int.get();

	lightingPass->getLightRef(1).position = pl2_pos.get();
	lightingPass->getLightRef(1).diffuseColor = pl2_diff.get();
	lightingPass->getLightRef(1).specularColor = pl2_spe.get();
	lightingPass->getLightRef(1).intensity = pl2_int.get();
	lightingPass->getLightRef(1).radius = pl2_rad.get();

	ssaoPass->setOcculusionRadius(ao_rad.get());
	ssaoPass->setDarkness(ao_dark.get());

	shadowLightPass->setAmbientColor(sha_amb.get());
	shadowLightPass->setDiffuseColor(sha_dif.get());
	shadowLightPass->setDarkness(sha_dark.get());

	bloomPass->setThreshold(thres.get());

	dofPass->setFoculRange(vec2(dof_focus.get()));
	dofPass->setEndPointsCoC(dof_coc.get());
	
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
	//dof.add(dof_blur.set("Max Blur", 12.f, 0.0, 100.f));
	dof.add(dof_focus.set("Focus", 0.2, 0., 1.));
	dof.add(dof_coc.set("coc", vec2(1., 0.5), vec2(0.), vec2(1.)));
	panel.add(dof);
	
	panel.minimizeAll();

}

void ofApp::keyPressed(int key) {
	if (key == 's') isShowPanel = !isShowPanel;
}
