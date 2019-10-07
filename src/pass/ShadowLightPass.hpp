#pragma once
#include "RenderPass.hpp"
#include "ofNode.h"

namespace ofxDeferred {

	class ShadowLightPass : public RenderPass, public ofNode {
	private:

		ofShader shader;
		ofShader linearDepthShader;
		bool useShader = false;
		ofFbo shadowMap;

		const glm::mat4 biasMat = glm::mat4(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

		glm::mat4 projection;
		glm::mat4 modelView;
		glm::mat4 depthMVP;
		glm::mat4 shadowTransMat;
		glm::vec3 directionInView;
		float linearDepthScalar;

		glm::mat4 invCamMat;
		float camFar;

		ofParameter<float> nearClip;
		ofParameter<float> farClip;

		// view port
		ofParameter<float> viewPortSize;

		ofParameter<float> darkness;
		ofParameter<ofFloatColor> ambientColor;
		ofParameter<ofFloatColor> diffuseColor;
		ofParameter<ofFloatColor> specularColor;

		ofParameter<glm::vec3> pos;
		ofParameter<glm::vec3> center;
		ofParameter<float> biasScalar;

		bool isLighting;
	public:
		using Ptr = std::shared_ptr<ShadowLightPass>;
		ShadowLightPass(const glm::vec2& size);

		void beginShadowMap(bool bUseOwnShader = false);
		void endShadowMap();

		void debugDraw(const glm::vec2& p, const glm::vec2& size) override;
		ofFbo& getShadowMap() { return shadowMap; };

		void update(const ofCamera& cam) override;
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		void setDarkness(float _darkness) { darkness = _darkness; }
		void setFar(float _farClip) { farClip = _farClip; }
		void setNear(float _nearClip) { nearClip = _nearClip; }
		void setViewPortSize(float size) { viewPortSize = size; }
		void setGlobalPosition(const glm::vec3& pos) {
			this->pos = pos;
			ofNode::setGlobalPosition(pos);
		}
		void lookAt(const glm::vec3& dir) {
			this->center = dir;
			ofNode::lookAt(dir);
		}

		float getLinearScalar() { return linearDepthScalar; }
		
		void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
		void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
		
		void disableLighting() { isLighting = false; }
		void enableLighting() { isLighting = true; }

	};

}
