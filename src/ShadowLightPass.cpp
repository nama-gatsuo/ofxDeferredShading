#include "ShadowLightPass.hpp"

using namespace ofxDeferred;

ShadowLightPass::ShadowLightPass(const glm::vec2& size) : RenderPass(size, "ShadowLightPass") {

	// create shadow map fbo
	ofFbo::Settings s;
	s.width = 2048;
	s.height = 2048;
	s.minFilter = GL_LINEAR;
	s.maxFilter = GL_LINEAR;
	s.wrapModeVertical = GL_CLAMP_TO_EDGE;
	s.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
	s.internalformat = GL_R32F;
	s.useDepth = true;
	s.useStencil = true;
	s.depthStencilAsTexture = true;

	shadowMap.allocate(s);

	group.add(nearClip.set("near_clip", 0., 0., 10000.f));
	group.add(farClip.set("far_clip", 5000., 1., 10000.f));
	
	linearDepthScalar = 1.f / (farClip - nearClip);

	group.add(viewPortSize.set("view_port_size", 1024.f, 2., 2048.));
	group.add(darkness.set("darkness", 0.8, 0., 1.));

	group.add(ambientColor.set("ambient_color", ofFloatColor(0.6)));
	group.add(diffuseColor.set("diffuse_color", ofFloatColor(0.9)));
	
	// load shader
	shader.load(passThruPath, shaderPath + "shadow/shadowLight.frag");
	linearDepthShader.load(shaderPath + "gbuffer.vert", shaderPath + "shadow/LinearDetph.frag");

}

void ShadowLightPass::beginShadowMap(bool bUseOwnShader) {

	useShader = bUseOwnShader;

	// update view matrix of depth camera
	projection = glm::ortho<float>(-viewPortSize, viewPortSize, viewPortSize, -viewPortSize, nearClip, farClip);
	modelView = glm::inverse(glm::translate(getGlobalPosition()) * glm::toMat4(getGlobalOrientation()));
	depthMVP = projection * modelView;
	shadowMap.begin();

	ofClear(0);
	ofBackground(255, 0, 0);
	ofEnableDepthTest();

	ofPushView();
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(projection);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadMatrix(modelView);
	
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	if (!useShader) {
		linearDepthShader.begin();
		linearDepthShader.setUniform1f("lds", linearDepthScalar);
	}

}

void ShadowLightPass::endShadowMap() {

	if (!useShader) linearDepthShader.end();

	//glDisable(GL_CULL_FACE);
	
	ofDisableDepthTest();
	ofPopView();

	shadowMap.end();

}

void ShadowLightPass::debugDraw() {
	shadowMap.getTexture().draw(0, 0, 256, 256);
}

void ShadowLightPass::update(const ofCamera &cam) {

	linearDepthScalar = 1.f / (farClip - nearClip);
	shadowTransMat = biasMat * depthMVP * glm::inverse(cam.getModelViewMatrix());
	directionInView = (glm::inverse(glm::transpose(cam.getModelViewMatrix())) * glm::vec4(getLookAtDir(), 0.f));

}

void ShadowLightPass::render(ofFbo &readFbo, ofFbo &writeFbo, GBuffer &gbuffer) {

	writeFbo.begin();

	ofClear(0);

	shader.begin();
	shader.setUniformTexture("lightDepthTex", shadowMap.getTexture(), 1);
	shader.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 2);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 3);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 4);
	shader.setUniformMatrix4f("shadowTransMat", shadowTransMat);
	
	shader.setUniform3f("lightDir", directionInView);
	shader.setUniform1f("darkness", darkness);
	shader.setUniform1f("lds", linearDepthScalar);
	shader.setUniform1f("near", nearClip);

	shader.setUniform4f("ambient", ambientColor);
	shader.setUniform4f("diffuse", diffuseColor);
	readFbo.draw(0, 0);

	shader.end();

	writeFbo.end();
}
