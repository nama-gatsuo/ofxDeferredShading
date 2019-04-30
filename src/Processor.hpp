#pragma once
#include "GBuffer.hpp"

namespace ofxDeferred {

	class RenderPass {
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		RenderPass(const glm::vec2& sz, const std::string& n);
		virtual void update(const ofCamera& cam) = 0;
		virtual void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) = 0;

		void setEnabled(bool enabled) { this->enabled = enabled; }
		bool getEnabled() const { return enabled; }
		const std::string& getName() const { return name; }

		ofParameterGroup& getParameters() { return group; }

	protected:
		std::string name;
		ofParameterGroup group;
		ofParameter<bool> enabled;
		const glm::vec2 size;
	};

	class Processor : public ofBaseDraws {
	public:
		using Ptr = std::shared_ptr<Processor>;

		Processor();
		void init(unsigned w = ofGetWidth(), unsigned h = ofGetHeight());

		void begin(ofCamera& cam, bool bUseOwnShader = false);
		void end(bool autoDraw = true);

		void draw(float x = 0., float y = 0.) const;
		void draw(float x, float y, float w, float h) const;
		float getWidth() const { return width; }
		float getHeight() const { return height; }

		void debugDraw();

		template<class T>
		std::shared_ptr<T> createPass() {
			std::shared_ptr<T> pass = std::make_shared<T>(glm::vec2(width, height));
			passes.push_back(pass);
			params.add(pass->getParameters());
			return pass;
		}

		unsigned size() const { return passes.size(); }
		RenderPass::Ptr operator[](unsigned i) const { return passes[i]; }
		std::vector<RenderPass::Ptr>& getPasses() { return passes; }
		
		const ofFbo& getFbo() { return pingPong[currentReadFbo]; }
		GBuffer& getGBuffer() { return gbuffer; }

		ofParameterGroup& getParameters() { return params; }

	private:
		void process();
		unsigned getNumProcessedPasses() const { return numProcessedPasses; }

		unsigned currentReadFbo;
		unsigned numProcessedPasses;
		unsigned width, height;

		GBuffer gbuffer;
		ofFbo pingPong[2];
		std::vector<RenderPass::Ptr> passes;

		ofParameterGroup params;
	};

}
