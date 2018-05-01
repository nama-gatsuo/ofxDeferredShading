#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	archi.setup();
	cells.setup();

	setupDeferred();
	setupGui();

}

//--------------------------------------------------------------
void ofApp::update() {
	shadowLightPass->setDirection(glm::normalize(glm::vec3(cos(ofGetElapsedTimef()), -1.5f, sin(ofGetElapsedTimef()))));

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
	deferred.end();

	if (isShowPanel) {
		shadowLightPass->debugDraw();
		deferred.debugDraw();
		panel.draw();
	}

}

void ofApp::setupDeferred() {
	deferred.init(ofGetWidth(), ofGetHeight());
	ssaoPass = deferred.createPass<SsaoPass>();

	lightingPass = deferred.createPass<PointLightPass>();
	ofxDeferred::PointLight dlight;
	dlight.ambientColor = ofFloatColor(0.005);
	lightingPass->addLight(dlight);

	dlight.ambientColor = ofFloatColor(0.0);
	lightingPass->addLight(dlight);

	shadowLightPass = deferred.createPass<ShadowLightPass>();
	shadowLightPass->setDarkness(0.9f);
	shadowLightPass->setViewPortSize(1600.f);

	hdrPass = deferred.createPass<HdrBloomPass>();
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

	dofPass->setFocus(dof_focal.get());
	dofPass->setMaxBlur(dof_blur.get());
	dofPass->setAperture(dof_ape.get());
}

void ofApp::setupGui() {
	isShowPanel = false;

	panel.setup();
	pl1.setName("Point light 1");
	pl1.add(pl1_pos.set("Position", glm::vec3(500, 500, 500), glm::vec3(-1000), glm::vec3(1000)));
	pl1.add(pl1_diff.set("Diffuse Color", ofFloatColor(0.4)));
	pl1.add(pl1_spe.set("Specular Color", ofFloatColor(1.0)));
	pl1.add(pl1_rad.set("Radius", 500, 100, 2000));
	pl1.add(pl1_int.set("Intensity", 1.2, 0.1, 3.0));
	panel.add(pl1);

	pl2.setName("Point light 2");
	pl2.add(pl2_pos.set("Position", glm::vec3(-600, 700, 200), glm::vec3(-1000), glm::vec3(1000)));
	pl2.add(pl2_diff.set("Diffuse Color", ofFloatColor(0.4)));
	pl2.add(pl2_spe.set("Specular Color", ofFloatColor(1.0)));
	pl2.add(pl2_rad.set("Radius", 500, 100, 2000));
	pl2.add(pl2_int.set("Intensity", 1.2, 0.1, 3.0));
	panel.add(pl2);

	ao.setName("Ambient Occlusion");
	ao.add(ao_rad.set("Occlusion Radius", 5.0, 0.1, 100.0));
	ao.add(ao_dark.set("Darkness", 0.8, 0.1, 5.0));
	panel.add(ao);

	shadow.setName("Shadow Light");
	shadow.add(sha_amb.set("Ambient", ofFloatColor(0.2)));
	shadow.add(sha_dif.set("Diffuse", ofFloatColor(0.7)));
	panel.add(shadow);

	dof.setName("Defocus Blur");
	dof.add(dof_blur.set("Max Blur", 0.5, 0.0, 1.0));
	dof.add(dof_ape.set("Aperture", 0.1, 0.0, 1.0));
	dof.add(dof_focal.set("Focus Distance", 0.2, 0.0, 1.0));
	panel.add(dof);
}

void ofApp::keyPressed(int key) {
	if (key == 's') isShowPanel = !isShowPanel;
}
