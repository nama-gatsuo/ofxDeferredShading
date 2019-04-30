#include "SsaoPass.hpp"

using namespace ofxDeferred;

SsaoPass::SsaoPass(const glm::vec2& size) :
	RenderPass(size, "SsaoPass"), kernelSize(32), blur(size)
{
	
	for (int i = 0; i < kernelSize; i++) {
		glm::vec3 sample = glm::normalize(glm::vec3(
			ofRandom(-1., 1.), ofRandom(-1., 1.), ofRandom(0., 1.)
		));
		sample *= ofRandom(1.);
		float s = float(i) / kernelSize;
		s = 0.1 + s * s * 0.9;
		sample *= s;
		ssaoKernel.push_back(sample);
	}

	ofFloatPixels pix;
	pix.allocate(4, 4, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < 16; i++) {
		pix[i * 3 + 0] = ofRandom(-1., 1.);
		pix[i * 3 + 1] = ofRandom(-1., 1.);
		pix[i * 3 + 2] = 0.;
	}
	noiseTex.setFromPixels(pix);
	noiseTex.update();

	blur.setup(3, 1.f);
	ssao.allocate(size.x, size.y, GL_R16F);
	blurred.allocate(size.x, size.y, GL_R16F);

	calcAo.load(passThruPath, shaderPath + "ao/calcAo.frag");
	applyAo.load(passThruPath, shaderPath + "ao/applyAo.frag");

	group.add(radius.set("occlusion_radius", 5., 0., 50.));
	group.add(darkness.set("darkness", 1., 0., 1.));
}

void SsaoPass::update(const ofCamera& cam) {
	projection = cam.getProjectionMatrix(ofRectangle(0, 0, size.x, size.y));
}

void SsaoPass::render(ofFbo &readFbo, ofFbo &writeFbo, GBuffer &gbuffer) {

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
		readFbo.draw(0, 0);
		calcAo.end();
	}
	ssao.end();

	blur.blur(ssao, blurred);

	writeFbo.begin();
	ofClear(0);
	{
		applyAo.begin();
		applyAo.setUniformTexture("ssao", blurred.getTexture(), 1);
		applyAo.setUniform1f("darkness", darkness);
		readFbo.draw(0, 0);
		applyAo.end();
	}
	writeFbo.end();

}
