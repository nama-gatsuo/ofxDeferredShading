#include "GBuffer.hpp"

using namespace ofxDeferred;

void GBuffer::setup(int w, int h) {
	ofFbo::Settings s;
	s.width = w;
	s.height = h;
	s.minFilter = GL_NEAREST;
	s.maxFilter = GL_NEAREST;
	s.colorFormats.push_back(GL_RGBA32F); // color + stencil
	s.colorFormats.push_back(GL_RGBA32F); // vertex coord
	s.colorFormats.push_back(GL_RGBA32F); // depth + normal

	s.useDepth = true;
	s.useStencil = false;
	s.depthStencilAsTexture = false;
	s.numSamples = 0;

	fbo.allocate(s);

	shader.load(shaderPath + "gbuffer");
	debugShader.load(passThruPath, shaderPath + "alphaFrag.frag");

}

void GBuffer::begin(ofCamera &cam, bool bUseOtherShader) {

	cam.begin();
	cam.end();

	fbo.begin();

	std::vector<int> bufferInt;
	bufferInt.push_back(TYPE_ALBEDO);
	bufferInt.push_back(TYPE_POSITION);
	bufferInt.push_back(TYPE_DEPTH_NORMAL);
	fbo.setActiveDrawBuffers(bufferInt);

	ofClear(0, 0);
	ofPushView();

	ofRectangle viewPort(0, 0, fbo.getWidth(), fbo.getHeight());
	ofSetOrientation(ofGetOrientation(), cam.isVFlipped());
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(cam.getProjectionMatrix(viewPort));
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadMatrix(cam.getModelViewMatrix());

	ofPushStyle();
	ofEnableDepthTest();
	ofDisableAlphaBlending();
	
	bUseCommonShader = !bUseOtherShader;
	if (bUseCommonShader) {
		shader.begin();
		shader.setUniform1f("lds", 1.0 / (cam.getFarClip() - cam.getNearClip()));
		shader.setUniform1f("isShadow", 0);
	}

}

void GBuffer::end() const {

	if (bUseCommonShader) {
		shader.end();
	}
	ofDisableDepthTest();
	ofPopStyle();


	ofPopView();
	fbo.end();

}

void GBuffer::debugDraw(const glm::vec2& p, const glm::vec2& size) const {

	glm::vec2 pos(p);

	ofDisableAlphaBlending();
	fbo.getTexture(TYPE_ALBEDO).draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	fbo.getTexture(TYPE_POSITION).draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);
	fbo.getTexture(TYPE_DEPTH_NORMAL).draw(pos, size.x, size.y); pos += glm::vec2(size.x, 0);

	debugShader.begin();
	fbo.getTexture(TYPE_DEPTH_NORMAL).draw(pos, size.x, size.y);
	debugShader.end();

}
