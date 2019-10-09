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

		static const glm::mat4 biasMat;

		glm::mat4 projection, modelView;
		glm::mat4 depthMVP;
		glm::mat4 shadowTransMat;
		glm::vec3 directionInView;
		float linearDepthScalar;

		std::vector<glm::vec3> frustPos;
		ofRectangle viewRect;
		ofParameter<float> nearClip, farClip;

		ofParameter<float> darkness;
		ofParameter<ofFloatColor> ambientColor, diffuseColor, specularColor;

		ofParameter<glm::vec3> pos;
		ofParameter<glm::vec3> center;
		ofParameter<float> biasScalar;

		bool isLighting;
		
		std::vector<glm::vec3> calculateFrustnumVertices(const ofCamera& cam);
		void preUpdate(const ofCamera& cam);

	public:
		using Ptr = std::shared_ptr<ShadowLightPass>;
		ShadowLightPass(const glm::vec2& size);

		void beginShadowMap(const ofCamera& cam, bool bUseOwnShader = false);
		void endShadowMap();
		const ofTexture& getShadowMapTexture() const { return shadowMap.getTexture(); };

		void debugDraw(const glm::vec2& p, const glm::vec2& size) override;
		
		// Since update method from BasePass will be called after shadow map update,
		// another method which would be called befor shadow map update is needed
		void update(const ofCamera& cam) override {}
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;

		// Overriding methods from ofNode
		void setGlobalPosition(const glm::vec3& pos)  {
			this->pos = pos;
			ofNode::setGlobalPosition(pos);
		}
		void lookAt(const glm::vec3& dir) {
			this->center = dir;
			ofNode::lookAt(dir);
		}

		float getLinearScalar() { return linearDepthScalar; }
		
		void setDarkness(float _darkness) { darkness = _darkness; }
		void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
		void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
		
		void disableLighting() { isLighting = false; }
		void enableLighting() { isLighting = true; }

	};

}
