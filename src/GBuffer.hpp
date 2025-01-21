#pragma once
#include "ofVectorMath.h"
#include "ofBaseTypes.h"
#include "ofFbo.h"
#include "ofShader.h"
#include "ofCamera.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"

namespace ofxDeferred {

	const static std::string shaderPath("shaders/deferred/");
	const static std::string passThruPath(shaderPath + "passThru.vert");

	struct RenderInfo {
		float lds;
		bool isShadow;
		glm::vec4 clipPlane;
		glm::mat4 invCamMatrix;
	};

	class GBuffer {
	private:
		ofFbo fbo;
		ofShader shader;
		ofShader debugShader;
		bool bUseCommonShader;
	public:

		enum BufferType {
			TYPE_ALBEDO = 0, TYPE_POSITION = 1, TYPE_DEPTH_NORMAL = 2
		};
		GBuffer() {}
		void setup(int w = ofGetWidth(), int h = ofGetHeight());
		void begin(ofCamera& cam, bool bUseOthreShader = false);
		void end() const;
		void debugDraw(const glm::vec2& p, const glm::vec2& size) const;
		ofTexture& getTexture(int index) {
			return fbo.getTexture(index);
		}
		const ofTexture& getTexture(int index) const {
			return fbo.getTexture(index);
		}
		ofFbo& getFbo() {
			return fbo;
		}
	};
}
