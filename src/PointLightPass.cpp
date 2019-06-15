#include "PointLightPass.hpp"

using namespace ofxDeferred;

PointLightPass::PointLightPass(const glm::vec2& size) : RenderPass(size, "PointLightPass") {
	shader.load(passThruPath, shaderPath + "pointLight.frag");
	lightShader.load(shaderPath + "material/emissive");

	sphere = ofMesh::sphere(20);
	for (int i = 0; i < sphere.getNumVertices(); i++) {
		sphere.addColor(ofFloatColor(10.));
	}
}

void PointLightPass::update(const ofCamera &cam) {
	modelViewMatrix = cam.getModelViewMatrix();
}

void PointLightPass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {

	shader.begin();
	shader.setUniform1f("lAttenuation", 1.f);
	shader.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 1);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
	shader.setUniformTexture("readFbo", readFbo.getTexture(0), 4);
	shader.end();

	writeFbo.begin();
	ofClear(0);
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	readFbo.draw(0, 0);

	shader.begin();
	for (PointLight& light : lights) {
		shader.setUniform3f("lPosition", modelViewMatrix * glm::vec4(light.position.get(), 1.f)); // light position in view space
		shader.setUniform4f("lDiffuse", light.diffuseColor);
		shader.setUniform4f("lSpecular", light.specularColor);
		shader.setUniform1f("lIntensity", light.intensity);
		shader.setUniform1f("lRadius", light.radius);
		readFbo.draw(0, 0);
	}

	shader.end();

	ofDisableBlendMode();
	ofPopStyle();
	writeFbo.end();
}

void ofxDeferred::PointLightPass::drawLights(ofPolyRenderMode mode) {
	for (auto& light : lights) {
		ofPushStyle();
		float s = glm::clamp(5. * light.intensity, 2., 5.);
		ofSetColor(ofFloatColor(light.diffuseColor->r * s, light.diffuseColor->g * s, light.diffuseColor->b * s));

		ofPushMatrix();
		ofTranslate(light.position);
		ofScale(light.radius / 1000.);
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
		lightShader.setUniform4f("emissiveColor", light.diffuseColor);
		lightShader.setUniform1f("intensity", light.intensity);

		ofPushMatrix();
		ofTranslate(light.position);
		ofScale(light.radius / 1000.);
		sphere.draw(mode);
		ofPopMatrix();
	}

	lightShader.end();
}