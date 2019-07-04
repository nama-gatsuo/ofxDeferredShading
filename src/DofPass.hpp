#pragma once
#include "DeferredProcessor.hpp"
#include "BlurPass.h"

namespace ofxDeferred {
	class DofPass : public RenderPass {
	public:
		using Ptr = std::shared_ptr<DofPass>;

		DofPass(const glm::vec2& size);
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		void setEndPointsCoC(const glm::vec2& p) { endPointsCoC = p; }
		void setFoculRange(const glm::vec2& range) { foculRange = range; }
		void debugDraw();

	private:
		void applySmallBlur(const ofTexture& read, ofFbo& write);

		ofShader downSample;
		ofShader calcNearCoc;
		ofShader smallBlur;
		ofShader applyDof;
		ofShader debugShader;
		BlurPass blur;

		ofFbo shrunk;
		ofFbo shrunkBlurred;
		ofFbo nearCoC;
		ofFbo colorBlurred;

		ofParameter<glm::vec2> endPointsCoC;
		ofParameter<glm::vec2> foculRange;

	
	};
}