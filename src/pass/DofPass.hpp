#pragma once
#include "RenderPass.hpp"
#include "BlurPass.h"
#include "AtomicCounterBuffer.h"
#include "ofVboMesh.h"

namespace ofxDeferred {
	class DofPass : public RenderPass {
	public:
		using Ptr = std::shared_ptr<DofPass>;

		DofPass(const glm::vec2& size);
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		void setEndPointsCoC(const glm::vec2& p) { endPointsCoC = p; }
		void setFoculRange(const glm::vec2& range) { foculRange = range; }
		void debugDraw(const glm::vec2& p, const glm::vec2& size) override;

		void onTexParamChanged(int&);
		void onMaxBokehCountChanged(int&);

		void refer(DofPass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			endPointsCoC.makeReferenceTo(pass.endPointsCoC);
			foculRange.makeReferenceTo(pass.foculRange);
			isActiveBokeh.makeReferenceTo(pass.isActiveBokeh);
			maxBokehCount.makeReferenceTo(pass.maxBokehCount);
			bokehCocThres.makeReferenceTo(pass.bokehCocThres);
			bokehLumThres.makeReferenceTo(pass.bokehLumThres);
			maxBokehRadius.makeReferenceTo(pass.maxBokehRadius);
			bokehDepthCutoff.makeReferenceTo(pass.bokehDepthCutoff);
			bokehShapeId.makeReferenceTo(pass.bokehShapeId);
			blur.refer(pass.blur);
		}

	private:
		void applySmallBlur(const ofTexture& read, ofFbo& write);
		void calcBokeh(const ofTexture& read);
		void renderBokeh();

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
		ofFbo depth;

		ofParameter<glm::vec2> endPointsCoC;
		ofParameter<glm::vec2> foculRange;

		// Variables for bokeh calculation
		ofParameterGroup bokehGroup;
		ofParameter<bool> isActiveBokeh;
		ofParameter<int> maxBokehCount;
		ofParameter<float> bokehCocThres;
		ofParameter<float> bokehLumThres;
		ofParameter<float> maxBokehRadius;
		ofParameter<float> bokehDepthCutoff;
		ofParameter<int> bokehShapeId;
		AtomicCounterBuffer atomicBuffer;
		ofShader detectBokehShader;
		ofShader bokehRenderShader;
		ofShader bokehShapingShader;
		ofTexture bokehColorTex;
		ofTexture bokehPosDepthCocTex;
		ofFbo bokehShapeTex;
		
		ofVboMesh vbo;
	};
}