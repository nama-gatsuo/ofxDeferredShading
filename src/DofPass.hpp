#pragma once
#include "Processor.hpp"
#include "Blur.h"

namespace ofxDeferred {
	class DofPass : public RenderPass {
	private:
		ofShader downSample;
		ofShader calcNearCoc;
		ofShader smallBlur;
		ofShader applyDof;
		Blur blur;

		float maxBlur;

		ofFbo shrunk;
		ofFbo shrunkBlurred;
		ofFbo nearCoC;
		ofFbo colorBlurred;

		float blurRadius;
		glm::vec2 endPointsCoC;
		glm::vec2 foculRange;

	public:
		using Ptr = std::shared_ptr<DofPass>;

		DofPass(const glm::vec2& size);
		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
		void setMaxBlur(float maxBlur) { this->maxBlur = maxBlur; }

		void setEndPointsCoC(const glm::vec2& p) { endPointsCoC = p; }
		void setFoculRange(const glm::vec2& range) { foculRange = range; }
	};
}