#pragma once
#include "Processor.hpp"

namespace ofxDeferred {
	class EdgePass : public RenderPass {
	private:
		ofShader shader;
		ofFloatColor edgeColor;
		ofFloatColor bgColor;
		bool useReadColor;

	public:
		using Ptr = std::shared_ptr<EdgePass>;

		EdgePass(const glm::vec2& size);
		void setEdgeColor(const ofFloatColor& col) { edgeColor = col; }
		void setBackground(const ofFloatColor& col) { bgColor = col; }
		void setUseReadColor(bool active) { useReadColor = active; }
		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

	};
}

