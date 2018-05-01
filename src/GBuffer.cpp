#include "GBuffer.hpp"

using namespace ofxDeferred;

void GBuffer::setup(int w, int h) {
	ofFbo::Settings s;
	s.width = w;
	s.height = h;
	s.minFilter = GL_NEAREST;
	s.maxFilter = GL_NEAREST;
	s.colorFormats.push_back(GL_RGB); // color
	s.colorFormats.push_back(GL_RGBA32F); // vertex coord
	s.colorFormats.push_back(GL_RGBA32F); // depth + normal
	s.colorFormats.push_back(GL_RGB32F); // HDR map
	s.depthStencilAsTexture = true;
	s.useDepth = true;
	s.useStencil = true;

	fbo.allocate(s);

	shader.load("shader/gbuffer");
	debugShader.load("shader/gbuffer.vert", "shader/alphaFrag.frag");

}

void GBuffer::begin(ofCamera &cam, bool bUseOtherShader) {

	cam.begin();
	cam.end();

	fbo.begin();

	std::vector<int> bufferInt;
	bufferInt.push_back(TYPE_ALBEDO);
	bufferInt.push_back(TYPE_POSITION);
	bufferInt.push_back(TYPE_DEPTH_NORMAL);
	bufferInt.push_back(TYPE_HDR);
	fbo.setActiveDrawBuffers(bufferInt);

	ofClear(0, 0);
	ofPushView();

	ofRectangle viewPort(0, 0, fbo.getWidth(), fbo.getHeight());
	ofSetOrientation(ofGetOrientation(), cam.isVFlipped());
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(cam.getProjectionMatrix(viewPort));
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadMatrix(cam.getModelViewMatrix());

	bUseShader = !bUseOtherShader;
	if (bUseShader) {
		shader.begin();
		shader.setUniformMatrix4f("normalMatrix", glm::inverse(glm::transpose(cam.getModelViewMatrix())));
		shader.setUniform1f("farClip", cam.getFarClip());
		shader.setUniform1f("nearClip", cam.getNearClip());
	}

	ofPushStyle();
	ofEnableDepthTest();
	ofDisableAlphaBlending();

}

void GBuffer::end() {
	ofDisableDepthTest();
	ofPopStyle();

	if (bUseShader) shader.end();

	ofPopView();
	fbo.end();

}

void GBuffer::debugDraw() {
	ofDisableAlphaBlending();
	float w2 = ofGetViewportWidth();
	float h2 = ofGetViewportHeight();
	float ws = w2 * 0.25;
	float hs = h2 * 0.25;

	fbo.getTexture(TYPE_ALBEDO).draw(0, hs * 3, ws, hs);
	fbo.getTexture(TYPE_POSITION).draw(ws, hs * 3, ws, hs);
	fbo.getTexture(TYPE_DEPTH_NORMAL).draw(ws * 2, hs * 3, ws, hs);

	debugShader.begin();
	fbo.getTexture(TYPE_DEPTH_NORMAL).draw(ws * 3, hs * 3, ws, hs);
	debugShader.end();
	//fbo.getTexture(TYPE_HDR).draw(ws*3, hs*3, ws, hs);
}
