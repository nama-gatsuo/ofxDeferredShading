#pragma once
#include "Processor.hpp"

namespace ofxDeferred {
	class DofPass : public RenderPass {
	private:
		ofShader shader;
		float focus;
		float aperture;
		float maxBlur;
	public:
		using Ptr = shared_ptr<DofPass>;

		DofPass(const glm::vec2& size);
		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void setFocus(float focus) { this->focus = focus; }
		void setAperture(float aperture) { this->aperture = aperture; }
		void setMaxBlur(float maxBlur) { this->maxBlur = maxBlur; }
	};
}
