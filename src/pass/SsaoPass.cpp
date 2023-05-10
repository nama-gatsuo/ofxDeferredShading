#include "SsaoPass.hpp"

using namespace ofxDeferred;

SsaoPass::SsaoPass(const glm::vec2& size) :
	RenderPass(size, RenderPassRegistry::Ssao), kernelSize(32), blur(size, GL_R8)
{
	
	for (int i = 0; i < kernelSize; i++) {
		glm::vec3 sample = glm::normalize(glm::vec3(
			ofRandom(-1.f, 1.f), ofRandom(-1.f, 1.f), ofRandom(0.f, 1.f)
		));
		sample *= ofRandom(1.f);
		float s = float(i) / kernelSize;
		s = 0.1f + s * s * 0.9f;
		sample *= s;
		ssaoKernel.push_back(sample);
	}

	ofFloatPixels pix;
	pix.allocate(4, 4, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < 16; i++) {
		pix[i * 3 + 0] = ofRandom(-1.f, 1.f);
		pix[i * 3 + 1] = ofRandom(-1.f, 1.f);
		pix[i * 3 + 2] = 0.f;
	}
	noiseTex.setFromPixels(pix);
	noiseTex.update();

	blur.setSampleStep(1.f);
	blur.setBlurRes(3);
	ssao.allocate(size.x, size.y, GL_R8);
	ssao.getTexture().setRGToRGBASwizzles(true);
	blurred.allocate(size.x, size.y, GL_R8);
	blurred.getTexture().setRGToRGBASwizzles(true);

	calcAo.load(passThruPath, shaderPath + "ao/calcAo.frag");
	applyAo.load(passThruPath, shaderPath + "ao/applyAo.frag");

	group.add(radius.set("occlusion_radius", 5.f, 0.f, 50.f));
	group.add(darkness.set("darkness", 1.f, 0.f, 1.f));
}

void SsaoPass::update(const ofCamera& cam) {
	projection = cam.getProjectionMatrix(ofRectangle(0, 0, size.x, size.y));
}

void SsaoPass::refer(SsaoPass& pass) {
	enabled.makeReferenceTo(pass.enabled);
	radius.makeReferenceTo(pass.radius);
	darkness.makeReferenceTo(pass.darkness);
	blur.refer(pass.blur);
}

void SsaoPass::debugDraw(const glm::vec2& p, const glm::vec2& size) {
	ssao.draw(p, size.x, size.y);
	blurred.draw(p + glm::vec2(size.x, 0), size.x, size.y);
}

void SsaoPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {

	ssao.begin();
	ofClear(0);
	{
		calcAo.begin();
		calcAo.setUniform2f("size", size);
		calcAo.setUniform1f("radius", radius);
		calcAo.setUniformMatrix4f("projectionMatrix", projection);
		calcAo.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 1);
		calcAo.setUniformTexture("positionTex", gbuffer.getTexture(GBuffer::TYPE_POSITION), 2);
		calcAo.setUniformTexture("normalAndDepthTex", gbuffer.getTexture(GBuffer::TYPE_DEPTH_NORMAL), 3);
		calcAo.setUniformTexture("noiseTex", noiseTex.getTexture(), 4);
		calcAo.setUniform3fv("ssaoKernel", &ssaoKernel[0].x, ssaoKernel.size());
		read.draw(0, 0);
		calcAo.end();
	}
	ssao.end();

	blur.render(ssao.getTexture(), blurred, gbuffer);

	write.begin();
	ofClear(0);
	{
		applyAo.begin();
		applyAo.setUniformTexture("ssao", blurred.getTexture(), 1);
		calcAo.setUniformTexture("colorTex", gbuffer.getTexture(GBuffer::TYPE_ALBEDO), 2);
		applyAo.setUniform1f("darkness", darkness);
		read.draw(0, 0);
		applyAo.end();
	}
	write.end();

}
