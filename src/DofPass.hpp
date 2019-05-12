#pragma once
#include "DeferredProcessor.hpp"
#include "Blur.h"

namespace ofxDeferred {
	class DofPass : public RenderPass {
	private:
		ofShader downSample;
		ofShader calcNearCoc;
		ofShader smallBlur;
		ofShader applyDof;
		Blur blur;

		ofFbo shrunk;
		ofFbo shrunkBlurred;
		ofFbo nearCoC;
		ofFbo colorBlurred;

		ofParameter<glm::vec2> endPointsCoC;
		ofParameter<glm::vec2> foculRange;

	public:
		using Ptr = std::shared_ptr<DofPass>;

		DofPass(const glm::vec2& size);
		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void setEndPointsCoC(const glm::vec2& p) { endPointsCoC = p; }
		void setFoculRange(const glm::vec2& range) { foculRange = range; }
	};
}