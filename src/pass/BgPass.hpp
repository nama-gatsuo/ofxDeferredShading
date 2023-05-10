#pragma once
#include "RenderPass.hpp"

namespace ofxDeferred {
	class BgPass : public RenderPass {
	private:
		ofFbo bg;
		ofParameter<ofFloatColor> fillColor;
	public:
		using Ptr = std::shared_ptr<BgPass>;

		BgPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Bg) {
			bg.allocate(size.x, size.y, GL_RGBA);
			group.add(fillColor.set("fill_color", ofFloatColor(0.1)));
			fillColor.addListener(this, &BgPass::onParamChanged);
		}
		void begin() {
			bg.begin();
			ofClear(fillColor.get());
		}
		void end() {
			bg.end();
		}

		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& writeFbo, const GBuffer& gbuffer) override {
			writeFbo.begin();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			ofClear(fillColor.get());

			bg.draw(0, 0);
			gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
			
			ofDisableBlendMode();
			writeFbo.end();
		}
		void onParamChanged(ofFloatColor& c) {
			begin();
			end();
		}

		void refer(BgPass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			fillColor.makeReferenceTo(pass.fillColor);
		}
	};
}


