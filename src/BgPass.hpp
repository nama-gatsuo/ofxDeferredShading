#pragma once
#include "DeferredProcessor.hpp"

namespace ofxDeferred {
	class BgPass : public RenderPass {
	private:
		ofFbo bg;
		ofParameter<ofFloatColor> fillColor;
	public:
		using Ptr = std::shared_ptr<BgPass>;

		BgPass(const glm::vec2& size) : RenderPass(size, "BgPass") {
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
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			ofClear(fillColor.get());

			bg.draw(0, 0);
			gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
			
			glDisable(GL_BLEND);
			writeFbo.end();
		}
		void onParamChanged(ofFloatColor& c) {
			begin();
			end();
		}
	};
}


