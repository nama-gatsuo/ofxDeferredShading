#include "DeferredProcessor.hpp"

using namespace ofxDeferred;

Processor::Processor() : 
	isProcessing(true),width(0),height(0) {
	params.setName("ofxDeferredShading");
}

void Processor::init(unsigned w, unsigned h) {
	width = w;
	height = h;

	ofFbo::Settings s;
	s.width = width;
	s.height = height;
	s.internalformat = GL_RGBA;
	s.numSamples = 0;
	s.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
	s.wrapModeVertical = GL_CLAMP_TO_EDGE;

	s.useDepth = true;
	s.useStencil = true;
	s.depthStencilAsTexture = false;

	pingPong.resize(s);

	gbuffer.setup(width, height);

	params.setName("deferred");
}

void Processor::begin(ofCamera& cam, bool bUseOwnShader) {

	for (auto pass : passes) {
		if (pass->getEnabled()) {
			pass->update(cam);
		}
	}

	gbuffer.begin(cam, bUseOwnShader);

}

void Processor::end(bool autoDraw) {

	gbuffer.end();

	if (isProcessing) {
		ofPushStyle();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);
		ofSetColor(255);
		process();
		if (autoDraw) draw();
		glPopAttrib();
		ofPopStyle();
	}
	
}

void Processor::draw(float x, float y) const {
	draw(x, y, width, height);
}

void Processor::draw(float x, float y, float w, float h) const {
	pingPong.src->draw(0, 0, w, h);
}

void Processor::debugDraw() {
	glm::vec2 s(ofGetViewportWidth() * 0.25, ofGetViewportHeight() * 0.25);
	glm::vec2 p(0, ofGetHeight() - s.y);

	gbuffer.debugDraw(p, s);
}

void Processor::process() {

	int numProcessedPasses = 0;
	for (auto pass : passes) {
		if (pass->getEnabled()) {
			if (numProcessedPasses == 0) pass->render(gbuffer.getFbo().getTexture(), *(pingPong.dst), gbuffer);
			else pass->render(pingPong.src->getTexture(), *(pingPong.dst), gbuffer);
			
			pingPong.swap();
			numProcessedPasses++;
		}
	}

	if (numProcessedPasses == 0) {
		pingPong.dst->begin();
		ofClear(0.);
		gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
		pingPong.dst->end();
		pingPong.swap();
	}

}
