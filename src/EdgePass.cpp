#include "EdgePass.hpp"

using namespace ofxDeferred;

EdgePass::EdgePass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Edge) {
	shader.load(passThruPath, shaderPath + "edge.frag");
	group.add(edgeColor.set("edge_color", ofFloatColor(1.)));
	group.add(bgColor.set("bg_color", ofFloatColor(0.)));
	group.add(useReadColor.set("use_read_as_bg", false));
}

void EdgePass::render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {
	writeFbo.begin();
	ofClear(0);
	shader.begin();

	shader.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 1);
	shader.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
	shader.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
	shader.setUniform1i("useReadColor", useReadColor ? 1 : 0);
	shader.setUniform4f("bgColor", bgColor);
	shader.setUniform4f("edgeColor", edgeColor);

	readFbo.draw(0, 0);

	shader.end();
	writeFbo.end();
}
