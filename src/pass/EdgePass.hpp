#pragma once
#include "RenderPass.hpp"

namespace ofxDeferred {
	class EdgePass : public RenderPass {
	private:
		ofShader shader;
		ofParameter<ofFloatColor> edgeColor;
		ofParameter<ofFloatColor> bgColor;
		ofParameter<bool> useReadColor;
		ofParameter<float> normalEdgeStrength;
		ofParameter<float> depthEdgeStrength;

	public:
		using Ptr = std::shared_ptr<EdgePass>;

		EdgePass(const glm::vec2& size);
		void setEdgeColor(const ofFloatColor& col) { edgeColor = col; }
		const ofFloatColor& getEdgeColor() { return edgeColor.get(); }
		void setBackground(const ofFloatColor& col) { bgColor = col; }
		void setUseReadColor(bool active) { useReadColor = active; }
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		void refer(EdgePass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			edgeColor.makeReferenceTo(pass.edgeColor);
			bgColor.makeReferenceTo(pass.bgColor);
		}
	};
}

