#pragma once
#include "GBuffer.hpp"

namespace ofxDeferred {

	class RenderPassRegistry {
	public:
		const static std::string Bg;
		const static std::string Bloom;
		const static std::string Blur;
		const static std::string Dof;
		const static std::string Edge;
		const static std::string Fog;
		const static std::string PointLight;
		const static std::string ShadowLight;
		const static std::string Ssao;
	};

	class RenderPass {
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		RenderPass(const glm::vec2& sz, const std::string& n);
		virtual void update(const ofCamera& cam) = 0;
		virtual void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) = 0;
		virtual void debugDraw(const glm::vec2& p, const glm::vec2& size) {}

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

}
