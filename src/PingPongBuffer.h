#pragma once
#include "ofFbo.h"
#include "ofTypes.h"
#include <array>
#include "ofAppRunner.h"

namespace ofxDeferred {

	class PingPongBuffer {
	public:
		PingPongBuffer(const glm::ivec2& res = glm::ivec2(ofGetWidth(), ofGetHeight()), GLint intrenalFormat = GL_RGB) :
			res(res) {

			ofFboSettings s;
			s.width = res.x;
			s.height = res.y;
			s.numColorbuffers = 1;
			s.numSamples = 0;
			s.maxFilter = GL_LINEAR;
			s.minFilter = GL_LINEAR;
			s.useDepth = false;
			s.useStencil = false;
			s.internalformat = intrenalFormat;

			for (auto& b : buffers) {
				b.allocate(s);
			}

			src = &(buffers[0]);
			dst = &(buffers[1]);
		}
		PingPongBuffer(const ofFboSettings& settings) :
			res(settings.width, settings.height) {
			for (auto& b : buffers) {
				b.allocate(settings);
			}

			src = &(buffers[0]);
			dst = &(buffers[1]);
		}
		~PingPongBuffer() {}

		void resize(const ofFboSettings& settings) {
			res.y = settings.width;
			res.x = settings.height;

			for (auto& b : buffers) {
				b.clear();
				b.allocate(settings);
			}
		}
		void resize(int w, int h) {
			res.x = w;
			res.y = h;

			for (auto& b : buffers) {
				GLint intrenalFormat = b.getTexture().getTextureData().glInternalFormat;
				b.clear();

				ofFboSettings s;
				s.width = res.x;
				s.height = res.y;
				s.internalformat = intrenalFormat;

				b.allocate(s);
			}

		}

		void swap() {
			std::swap(src, dst);
		}
		const ofFbo& operator[](int i) const {
			return buffers[i];
		}

		int getWidth() const { return res.x; }
		int getHeight() const { return res.y; }
		GLint getInternalFormat() const {
			return buffers[0].getTexture().getTextureData().glInternalFormat;
		}

		ofFbo* src;
		ofFbo* dst;
	private:
		std::array<ofFbo, 2> buffers;
		glm::ivec2 res;
	};
}