#include "BlurPass.h"
#include "ofMath.h"
#include "ofGraphics.h"

using namespace ofxDeferred;

BlurPass::BlurPass(const glm::vec2& size, GLint colorFormat) : RenderPass(size, RenderPassRegistry::Blur), pp(size, colorFormat) {

	group.add(preShrink.set("preShrink", 2, 1, 8));
	group.add(sampleStep.set("sampleStep", 1., 0., 10.));
	group.add(blurRes.set("blurRes", 12, 1, 16));

	preShrink.addListener(this, &BlurPass::onPreShrinkChanged);
	sampleStep.addListener(this, &BlurPass::onSampleStepChanged);
	blurRes.addListener(this, &BlurPass::onBlurResChanged);

	pp.resize(size.x / (float)preShrink, size.y / (float)preShrink);

	coefficients = createGaussianWeights(blurRes, 0.2f);

	shrinkShader.load(passThruPath, shaderPath + "blur/shrink.frag");
	shader.load(passThruPath, shaderPath + "blur/blur.frag");

	shader.begin();
	shader.setUniform1i("blurRes", blurRes);
	shader.setUniform1f("sampleStep", sampleStep);
	shader.setUniform1fv("coefficients", coefficients.data(), coefficients.size());
	shader.setUniform1i("preShrink", preShrink);
	shader.end();

}

void BlurPass::render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) {
	glm::vec2 shrunkSize(size / (float)preShrink);

	ofDisableAlphaBlending();
	pp.dst->begin();
	ofClear(0);
	read.draw(0, 0, shrunkSize.x, shrunkSize.y);
	pp.dst->end();

	pp.swap();

	for (int i = 0; i < 2; i++) {
		pp.dst->begin();
		ofClear(0);
		shader.begin();
		shader.setUniform1i("isHorizontal", i);
		pp.src->draw(0, 0);
		shader.end();

		pp.dst->end();

		pp.swap();
	}

	write.begin();
	ofClear(0);
	pp.src->draw(0, 0, size.x, size.y);
	write.end();
}

void BlurPass::onPreShrinkChanged(int& _value) {
	int ps = glm::clamp(_value, 1, 8);
	pp.resize(size.x / (float)preShrink, size.y / (float)preShrink);
	shader.begin();
	shader.setUniform1i("preShrink", ps);
	shader.end();
}

void BlurPass::onBlurResChanged(int& _value) {
	int br = glm::clamp(blurRes.get(), 4, 16);
	coefficients = createGaussianWeights(br, 0.2f);
	shader.begin();
	shader.setUniform1i("blurRes", br);
	shader.setUniform1fv("coefficients", coefficients.data(), coefficients.size());
	shader.end();
}

void BlurPass::onSampleStepChanged(float& _value) {
	shader.begin();
	shader.setUniform1f("sampleStep", sampleStep.get());
	shader.end();
}

inline std::vector<float> BlurPass::createGaussianWeights(int radius, float variance) {
	int rowSize = 1 + radius;
	float sum = 0.f;
	std::vector<float> row;

	for (int i = 0; i < rowSize; i++) {

		float x = ofMap(i, 0, radius, 0, 1.2f);
		float w = Gaussian(x, 0, variance);

		row.push_back(w);

		if (i != 0) w *= 2.f;
		sum += w;
	}

	for (int i = 0; i < row.size(); i++) {
		row[i] /= sum;
	}

	return row;
}

inline float BlurPass::Gaussian(float x, float mean, float variance) {
	x -= mean;
	return (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2. * variance));
}
