#pragma once
#include "ofMain.h"
#include "Processor.hpp"

namespace ofxDeferred {

	using namespace glm;

	class ShadowLightPass : public RenderPass, public ofNode {
	private:
		ofShader shader;
		ofShader linearDepthShader;
		bool useShader = false;
		ofFbo shadowMap;

		const mat4 biasMat = glm::mat4(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

		mat4 projection;
		mat4 modelView;
		mat4 depthMVP;
		mat4 shadowTransMat;
		vec3 directionInView;
		float linearDepthScalar;

		float nearClip;
		float farClip;
		
		vec3 direction;

		// view port
		float viewPortSize = 1024.f;

		float darkness = 0.f;
		ofFloatColor ambientColor;
		ofFloatColor diffuseColor;
		ofFloatColor specularColor;

	public:
		using Ptr = shared_ptr<ShadowLightPass>;
		ShadowLightPass(const glm::vec2& size);

		void beginShadowMap(bool bUseOwnShader = false);
		void endShadowMap();

		void debugDraw();
		ofFbo& getShadowMap() { return shadowMap; };

		void update(const ofCamera& cam);
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void setDarkness(float _darkness) { darkness = _darkness; }
		void setFar(float _farClip) { farClip = _farClip; }
		void setNear(float _nearClip) { nearClip = _nearClip; }
		void setViewPortSize(float size) { viewPortSize = size; }

		float getLinearScalar() { return linearDepthScalar; }
		
		void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
		void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
		
	};

}
