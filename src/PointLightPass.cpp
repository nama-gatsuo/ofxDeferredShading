#include "PointLightPass.hpp"

using namespace ofxDeferred;

PointLightPass::PointLightPass(const glm::vec2& size) : RenderPass(size, "PointLightPass") {
	shader.load(passThruPath, shaderPath + "pointLight.frag");
	lightShader.load(shaderPath + "gbuffer.vert", shaderPath + "customShader.frag");

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

		shader.setUniform3f("lPosition", modelViewMatrix * glm::vec4(light.position, 1.f)); // light position in view space
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

void PointLightPass::drawLights(ofPolyRenderMode mode) {

	for (auto light : lights) {
		ofPushMatrix();
		ofTranslate(light.position);
		sphere.draw(mode);
		ofPopMatrix();
	}

}

void PointLightPass::drawLights(ofCamera& cam, bool isShadow, ofPolyRenderMode mode) {

	glm::mat4 normalMatrix = glm::inverse(glm::transpose(cam.getModelViewMatrix()));

	lightShader.begin();
	lightShader.setUniform1i("isShadow", isShadow ? 1 : 0);
	lightShader.setUniformMatrix4f("normalMatrix", normalMatrix);
	lightShader.setUniform1f("lds", 1. - (cam.getFarClip() - cam.getNearClip()));

	drawLights(mode);

	lightShader.end();

}
