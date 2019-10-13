#include "ShadowLightPass.hpp"
#include "ofMath.h"

using namespace ofxDeferred;

BoundingBox getBound(const std::vector<glm::vec3>& points) {
	glm::vec3 currentMin(points[0]), currentMax(points[0]);
	for (auto& p : points) {
		currentMin = glm::min(currentMin, p);
		currentMax = glm::max(currentMax, p);
	}
	return { currentMin, currentMax };
}

const glm::mat4 ShadowLightPass::biasMat(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

void transformVerts(std::vector<glm::vec3>& points, const glm::mat4& m) {
	for (auto& p : points) {
		p = m * glm::vec4(p, 1.);
	}
}

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

std::vector<glm::vec3> ShadowLightPass::calculateFrustumVertices(const ofCamera& cam) {
	
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

	transformVerts(pos, cam.getGlobalTransformMatrix());
	
	return pos;
}

void ShadowLightPass::preUpdate(const ofCamera& cam) {
	setGlobalPosition(pos);
	lookAt(center);
	frustPos = calculateFrustumVertices(cam);
	
	if (true) {
		// Uniform Shadow Map
		modelView = glm::lookAt(cam.getPosition(), cam.getPosition() + getLookAtDir(), cam.getLookAtDir());
		std::vector<glm::vec3> newFrustPos(frustPos);
		
		transformVerts(newFrustPos, modelView);
		
		// AABB(Axis aligned bounding box) of view frustum in LightSpace
		auto b = getBound(newFrustPos);
		projection = b.getFitMatrix();
		shadowTransMat = biasMat * projection * modelView * glm::inverse(cam.getModelViewMatrix());
		linearDepthScalar = 1.f / b.depth;
		nearClip = -b.max.z;

	} else {
		// LiSPSM: Light Space Perspective Shadow Mapping

		const glm::vec3 left = glm::cross(getLookAtDir(), cam.getLookAtDir());
		const glm::vec3 up = glm::normalize(glm::cross(left, getLookAtDir()));
		const glm::mat4 lightView = glm::lookAt(cam.getPosition(), cam.getPosition() + getLookAtDir(), up);
		// Transform view-frustum vertices form world into light space
		std::vector<glm::vec3> newFrustPos(frustPos);
		transformVerts(newFrustPos, lightView);
		auto b = getBound(newFrustPos);
		
		const float cosAngle = glm::dot(cam.getLookAtDir(), getLookAtDir());
		const float sinAngle = sqrt(1.f - cosAngle * cosAngle);
		const float factor = 1.f / sinAngle;
		const float zn = factor * cam.getNearClip();
		const float d = b.height;
		const float zf = zn + d * sinAngle;
		const float n = (zn + sqrt(zf * zn)) * factor;
		const float f = n + d;

		const glm::vec3 warpEyePos = cam.getPosition() - up * (n - cam.getNearClip());
		modelView = glm::lookAt(warpEyePos, warpEyePos + getLookAtDir(), up);

		glm::mat4 lispMat(
			1.f, 0, 0, 0,
			0, (f + n) / (f - n), 0, 1.f,
			0, 0, 1.f, 0,
			0, - 2.f * f * n / (f - n), 0, 0
		);

		newFrustPos.clear();
		newFrustPos = frustPos;
		transformVerts(newFrustPos, lispMat * modelView);

		// AABB of frustum vertices in warped space
		auto wb = getBound(newFrustPos);
		const glm::mat4 fitMat = wb.getFitMatrix();
		ofLogNotice("fit") << "\n" << fitMat;
		projection = fitMat * lispMat;
		shadowTransMat = biasMat * projection * modelView * glm::inverse(cam.getModelViewMatrix());
		linearDepthScalar = 1.f / d;
		nearClip = n;
	}
	
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
		ofMultMatrix(biasMat * projection * modelView);

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
