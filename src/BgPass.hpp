#pragma once
#include "Processor.hpp"

namespace ofxDeferred {
	class BgPass : public RenderPass {
	private:
		ofFbo bg;

	public:
		using Ptr = std::shared_ptr<BgPass>;

		BgPass(const glm::vec2& size) : RenderPass(size, "BgPass") {
			bg.allocate(size.x, size.y, GL_RGBA);
		}
		void begin() {
			bg.begin();
			ofClear(0);
		}
		void end() {
			bg.end();
		}

		void update(const ofCamera& cam) {}
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer) {
			writeFbo.begin();
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			ofClear(0);

			bg.draw(0, 0);
			gbuffer.getTexture(GBuffer::TYPE_ALBEDO).draw(0, 0);
			
			glDisable(GL_BLEND);
			writeFbo.end();
		}

	};
}


