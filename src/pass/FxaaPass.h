#pragma once
#include "RenderPass.hpp"

namespace ofxDeferred {
	class FxaaPass : public RenderPass {	
	public:
		using Ptr = std::shared_ptr<FxaaPass>;

		FxaaPass(const glm::vec2& size) : RenderPass(size, "FxaaPass") {
			shader.load(passThruPath, shaderPath + "fxaa.frag");

			group.add(lumaThreshold.set("lumaThreshold", 0.5f, 0.0f, 1.0f));
			group.add(mulReduce.set("mulReduce", 1.f / 128.f, 0.0f, 1.0f));
			group.add(minReduce.set("minReduce", 1.f / 8.f, 0.0f, 1.0f));
			group.add(maxSpan.set("maxSpan", 8.f, 0.0f, 10.0f));
			group.add(texelStep.set("texelStep", glm::vec2(1.f) , glm::vec2(0.f), glm::vec2(10.f)));

		}
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& writeFbo, const GBuffer& gbuffer) override {
			writeFbo.begin();
			shader.begin();
			shader.setUniforms(group);
			read.draw(0, 0);
			shader.end();
			writeFbo.end();
		}
		void refer(FxaaPass& pass) {
			lumaThreshold.makeReferenceTo(pass.lumaThreshold);
			mulReduce.makeReferenceTo(pass.mulReduce);
			minReduce.makeReferenceTo(pass.minReduce);
			maxSpan.makeReferenceTo(pass.maxSpan);
			texelStep.makeReferenceTo(pass.texelStep);
		}

	private:
		ofShader shader;
		ofParameter<float> lumaThreshold;
		ofParameter<float> mulReduce;
		ofParameter<float> minReduce;
		ofParameter<float> maxSpan;
		ofParameter<glm::vec2> texelStep;

	};
}


