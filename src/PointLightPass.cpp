#include "PointLightPass.hpp"

using namespace ofxDeferred;

int PointLight::lightNum = 0;

PointLightPass::PointLightPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::PointLight) {
	shader.load(passThruPath, shaderPath + "pointLight.frag");
	lightShader.load(shaderPath + "material/emissive");
	group.add(lightBrightness.set("lightBrightness", 2., 0., 50.));
	sphere = ofMesh::sphere(20);
	for (int i = 0; i < sphere.getNumVertices(); i++) {
		sphere.addColor(ofFloatColor(10.));
	}
}

void PointLightPass::update(const ofCamera &cam) {
	modelViewMatrix = cam.getModelViewMatrix();
}

void PointLightPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {

	std::vector<glm::vec3> posArray;
	std::vector<ofFloatColor> diffColArray;
	std::vector<ofFloatColor> specColArray;
	std::vector<float> intensityArray;
	std::vector<float> radiusArray;

	posArray.reserve(lights.size());
	diffColArray.reserve(lights.size());
	specColArray.reserve(lights.size());
	intensityArray.reserve(lights.size());
	radiusArray.reserve(lights.size());

	for (auto& light : lights) {
		if (light->isActive) {
			posArray.push_back(modelViewMatrix * glm::vec4(light->position.get(), 1.f));
			diffColArray.push_back(light->diffuseColor.get());
			specColArray.push_back(light->specularColor.get());
			intensityArray.push_back(light->intensity.get());
			radiusArray.push_back(light->radius.get());
		}
	}
	const int n = posArray.size();

	
	writeFbo.begin();
	ofClear(0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	shader.begin();
	shader.setUniform1f("lAttenuation", 1.f);
	shader.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 1);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
	shader.setUniform1i("lightNum", n);
	shader.setUniform3fv("lPosition", &posArray[0].x, n); // light position in view space
	shader.setUniform4fv("lDiffuse", &diffColArray[0].r, n);
	shader.setUniform4fv("lSpecular", &specColArray[0].r, n);
	shader.setUniform1fv("lIntensity", intensityArray.data(), n);
	shader.setUniform1fv("lRadius", radiusArray.data(), n);

	readFbo.draw(0, 0);

	shader.end();

	//readFbo.draw(0, 0);

	ofDisableBlendMode();
	writeFbo.end();
}

void ofxDeferred::PointLightPass::drawLights(ofPolyRenderMode mode) {
	for (auto& light : lights) {
		
		ofPushStyle();
		float s = 1.;
		if (light->isActive) s = lightBrightness;
		ofSetColor(ofFloatColor(light->diffuseColor->r * s, light->diffuseColor->g * s, light->diffuseColor->b * s));
		ofPushMatrix();
		ofTranslate(light->position);
		ofScale(light->radius / 1000.);
		sphere.draw(mode);
		ofPopMatrix();
		ofPopStyle();
	}
}

void ofxDeferred::PointLightPass::drawLights(float lds, bool isShadow, ofPolyRenderMode mode) {
	lightShader.begin();
	lightShader.setUniform1i("isShadow", isShadow ? 1 : 0);
	lightShader.setUniform1f("lds", lds);

	for (auto& light : lights) {
		if (light->isActive) lightShader.setUniform1f("lightBrightness", lightBrightness);
		else lightShader.setUniform1f("lightBrightness", 1.);
		
		lightShader.setUniform4f("emissiveColor", light->diffuseColor);
		lightShader.setUniform1f("intensity", light->intensity);	

		ofPushMatrix();
		ofTranslate(light->position);
		ofScale(light->radius / 1000.);
		sphere.draw(mode);
		ofPopMatrix();
	}
	lightShader.end();
}