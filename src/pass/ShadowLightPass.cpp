#include "ShadowLightPass.hpp"
#include "ofMath.h"

using namespace ofxDeferred;

ofRectangle getBound(const std::vector<glm::vec3>& points) {
	glm::vec2 currentMin(points[0]), currentMax(points[0]);
	for (auto& p : points) {
		currentMin = glm::min(currentMin, glm::vec2(p.x, p.y));
		currentMax = glm::max(currentMax, glm::vec2(p.x, p.y));
	}
	return ofRectangle(currentMin, currentMax);
}

const glm::mat4 ShadowLightPass::biasMat(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

ShadowLightPass::ShadowLightPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::ShadowLight), isLighting(true) {

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
	shadowMap.getTexture().setRGToRGBASwizzles(true);

	group.add(nearClip.set("near_clip", 0., 0., 10000.f));
	group.add(farClip.set("far_clip", 5000., 1., 10000.f));

	linearDepthScalar = 1.f / (farClip - nearClip);

	group.add(darkness.set("darkness", 0.8, 0., 1.));
	group.add(biasScalar.set("biasScalar", 0.005, 0.001, 0.05));

	group.add(ambientColor.set("ambient_color", ofFloatColor(0.6)));
	group.add(diffuseColor.set("diffuse_color", ofFloatColor(0.9)));

	group.add(pos.set("source_position", glm::vec3(100., -200., 100.), glm::vec3(-2048.), glm::vec3(2048.)));
	group.add(center.set("source_look", glm::vec3(0., 0., 0.), glm::vec3(-1024.), glm::vec3(1024.)));
	// load shader
	shader.load(passThruPath, shaderPath + "shadow/shadowLight.frag");
	linearDepthShader.load(shaderPath + "gbuffer.vert", shaderPath + "shadow/LinearDetph.frag");

}

std::vector<glm::vec3> ShadowLightPass::calculateFrustnumVertices(const ofCamera& cam) {
	
	const float farHeight = cam.getFarClip() * tan(ofDegToRad(cam.getFov()) * 0.5f);
	const float farWidth = farHeight * (size.x / size.y);
	const float nearHeight = cam.getNearClip() * tan(ofDegToRad(cam.getFov()) * 0.5f);
	const float nearWidth = nearHeight * (size.x / size.y);

	// Origrinal frastnum coordnates
	std::vector<glm::vec3> pos{
		glm::vec3(-nearWidth, -nearHeight, -cam.getNearClip()),
		glm::vec3( nearWidth, -nearHeight, -cam.getNearClip()),
		glm::vec3( nearWidth,  nearHeight, -cam.getNearClip()),
		glm::vec3(-nearWidth,  nearHeight, -cam.getNearClip()),
		glm::vec3(-farWidth,  -farHeight,  -cam.getFarClip()),
		glm::vec3( farWidth,  -farHeight,  -cam.getFarClip()),
		glm::vec3( farWidth,   farHeight,  -cam.getFarClip()),
		glm::vec3(-farWidth,   farHeight,  -cam.getFarClip())
	};

	// Transform frastnum coodinates into light space
	for (auto& p : pos) {
		p = cam.getGlobalTransformMatrix() * glm::vec4(p, 1.);
	}

	return pos;

}

void ShadowLightPass::preUpdate(const ofCamera& cam) {
	setGlobalPosition(pos);
	lookAt(center);

	frustPos = calculateFrustnumVertices(cam);
	std::vector<glm::vec3> newFrustPos(frustPos);

	glm::mat4 lightMV = glm::inverse(getGlobalTransformMatrix());
	for (auto& p : newFrustPos) {
		p = lightMV * glm::vec4(p, 1.);
	}
	viewRect = getBound(newFrustPos);

	modelView = glm::translate(-viewRect.getCenter()) * lightMV;
	projection = glm::ortho<float>(
		-viewRect.width * 0.5f, viewRect.width * 0.5f,
		viewRect.height * 0.5f, -viewRect.height * 0.5f,
		nearClip, farClip
	);

	depthMVP = projection * modelView;

	linearDepthScalar = 1.f / (farClip - nearClip);
	shadowTransMat = biasMat * depthMVP * glm::inverse(cam.getModelViewMatrix());
	directionInView = (glm::inverse(glm::transpose(cam.getModelViewMatrix())) * glm::vec4(getLookAtDir(), 0.f));
}


void ShadowLightPass::beginShadowMap(const ofCamera& cam, bool bUseOwnShader) {

	preUpdate(cam);

	useShader = bUseOwnShader;
	
	shadowMap.begin();

	ofClear(0);
	ofBackground(255, 0, 0);
	ofEnableDepthTest();

	ofPushView();

	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(projection);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadMatrix(modelView);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	if (!useShader) {
		linearDepthShader.begin();
		linearDepthShader.setUniform1f("lds", linearDepthScalar);
	}
	
}

void ShadowLightPass::endShadowMap() {

	if (!useShader) linearDepthShader.end();

	glDisable(GL_CULL_FACE);
	
	ofDisableDepthTest();
	ofPopView();

	shadowMap.end();

}

void ShadowLightPass::debugDraw(const glm::vec2& p, const glm::vec2& s) {

	ofDisableAlphaBlending();
	shadowMap.getTexture().draw(p, s.x, s.x);
	
	{
		// Draw view-frustum for debug
		ofPushStyle();
		ofPushMatrix();
		
		ofTranslate(p);
		ofScale(s.x, s.y, 0);
		ofMultMatrix(biasMat * depthMVP);

		ofSetColor(255, 0, 0);
		for (int i = 0; i < frustPos.size(); i++) {
			ofDrawBitmapString(ofToString(frustPos[i]), frustPos[i]);
		}
		ofDrawLine(frustPos[0], frustPos[4]);
		ofDrawLine(frustPos[1], frustPos[5]);
		ofDrawLine(frustPos[2], frustPos[6]);
		ofDrawLine(frustPos[3], frustPos[7]);
			
		ofDrawLine(frustPos[4], frustPos[5]);
		ofDrawLine(frustPos[5], frustPos[6]);
		ofDrawLine(frustPos[6], frustPos[7]);
		ofDrawLine(frustPos[7], frustPos[4]);

		ofPopMatrix();
		ofPopStyle();
	}
	

	ofEnableAlphaBlending();
}

void ShadowLightPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {

	write.begin();

	ofClear(0);

	shader.begin();
	shader.setUniformTexture("lightDepthTex", shadowMap.getTexture(), 1);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
	shader.setUniformMatrix4f("shadowTransMat", shadowTransMat);
	
	shader.setUniform3f("lightDir", directionInView);
	shader.setUniform1f("darkness", darkness);
	shader.setUniform1f("lds", linearDepthScalar);
	shader.setUniform1f("near", nearClip);
	
	shader.setUniform1f("biasScalar", biasScalar);
	shader.setUniform1i("isLighting", isLighting ? 1 : 0);
	shader.setUniform4f("ambient", ambientColor);
	shader.setUniform4f("diffuse", diffuseColor);
	read.draw(0, 0);

	shader.end();

	write.end();
}
