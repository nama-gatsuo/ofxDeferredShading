#pragma once
#include "RenderPass.hpp"
#include "PingPongBuffer.h"

namespace ofxDeferred {

	class Processor : public ofBaseDraws {
	public:
		using Ptr = std::shared_ptr<Processor>;

		Processor();
		void init(unsigned w = ofGetWidth(), unsigned h = ofGetHeight());

		void begin(ofCamera& cam, bool bUseOwnShader = false);
		void end(bool autoDraw = true);

		void draw(float x = 0.f, float y = 0.f) const override;
		void draw(float x, float y, float w, float h) const override;
		float getWidth() const override { return width; }
		float getHeight() const override { return height; }

		void debugDraw();

		template<class T>
		std::shared_ptr<T> createPass() {
			std::shared_ptr<T> pass = std::make_shared<T>(glm::vec2(width, height));
			passes.push_back(pass);
			params.add(pass->getParameters());
			return pass;
		}

		size_t size() const { return passes.size(); }
		RenderPass::Ptr operator[](unsigned i) const { return passes[i]; }
		std::vector<RenderPass::Ptr>& getPasses() { return passes; }
		
		const ofFbo& getFbo() const { return *(pingPong.src); }
		const GBuffer& getGBuffer() const { return gbuffer; }

		ofParameterGroup& getParameters() { return params; }

		void enableProcessing() { isProcessing = true; }
		void disableProcessing() { isProcessing = false; }
	private:
		void process();

		unsigned width, height;

		GBuffer gbuffer;
		PingPongBuffer pingPong;
		std::vector<RenderPass::Ptr> passes;

		ofParameterGroup params;
		bool isProcessing;
	};

}
