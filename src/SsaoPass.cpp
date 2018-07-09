#include "SsaoPass.hpp"

using namespace ofxDeferred;

SsaoPass::SsaoPass(const glm::vec2& size) : RenderPass(size, "SsaoPass") {
	shader.load("shader/vfx/PassThru.vert", "shader/vfx/Ssao.frag");
}

void SsaoPass::update(ofCamera& cam) {
	projection = cam.getProjectionMatrix();
}

void SsaoPass::render(ofFbo &readFbo, ofFbo &writeFbo, GBuffer &gbuffer) {

	writeFbo.begin();
	ofPushStyle();
	ofClear(0);

	shader.begin();
	shader.setUniform2f("size", size);
	shader.setUniform1f("radius", radius);
	shader.setUniform1f("darkness", darkness);
	shader.setUniformMatrix4f("projectionMatrix", projection);
	shader.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 1);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
	readFbo.draw(0, 0);
	shader.end();

	ofPopStyle();
	writeFbo.end();

}
