#pragma once
#include "RenderPass.hpp"
#include "ofNode.h"

namespace ofxDeferred {

	class BoundingBox {
	public:
		BoundingBox(const glm::vec3& p0, const glm::vec3& p1) :
			min(p0), max(p1),
			width(abs(max.x - min.x)),
			height(abs(max.y - min.y)),
			depth(abs(max.z - min.z))
		{}
		const glm::vec3 min, max;
		const float width, height, depth;
		glm::mat4 getFitMatrix() {
			glm::mat4 fitMat(
				2.f / width, 0, 0, 0,
				0, 2.f / height, 0, 0,
				0, 0, 2.f / depth, 0,
				-(max.x + min.x) / width, -(max.y + min.y) / height, -(max.z + min.z) / depth, 1.f
			);
			return glm::scale(glm::vec3(1, 1, -1)) * fitMat;
		}
	};

	class ShadowLightPass : public RenderPass, public ofNode {
	private:

		ofShader shader;
		ofShader linearDepthShader;
		bool useShader = false;
		ofFbo shadowMap;

		static const glm::mat4 biasMat;

		glm::mat4 projection, modelView;
		glm::mat4 shadowTransMat;
		glm::vec3 directionInView;
		glm::vec2 sunPosInScreen;
		float linearDepthScalar;

		std::vector<glm::vec3> frustPos;
		//ofParameter<float> nearClip, farClip;
		float nearClip;
		ofParameter<float> darkness;
		ofParameter<ofFloatColor> ambientColor, diffuseColor, specularColor;

		ofParameter<glm::vec3> pos;
		ofParameter<glm::vec3> center;
		ofParameter<float> biasScalar;
		
		float camFovy, camFarClip;
		ofParameter<bool> isShading;
		ofParameter<bool> isVolume;
		ofParameter<bool> isDrawSun;
		ofParameter<bool> isFrontCulling;
		ofParameter<float> scattering;
		ofParameter<float> volumeStrength;

		std::vector<glm::vec3> calculateFrustumVertices(const ofCamera& cam);
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

		float getLinearScalar() const { return linearDepthScalar; }
		
		void setDarkness(float _darkness) { darkness = _darkness; }
		void setAmbientColor(const ofFloatColor& color) { ambientColor = color; }
		void setDiffuseColor(const ofFloatColor& color) { diffuseColor = color; }
		void setVolume(bool useVolume) { isVolume.set(useVolume); }
		void setVolumeStrength(float strength) { volumeStrength.set(strength); }

		void refer(ShadowLightPass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			darkness.makeReferenceTo(pass.darkness);
			ambientColor.makeReferenceTo(pass.ambientColor);
			diffuseColor.makeReferenceTo(pass.diffuseColor);
			specularColor.makeReferenceTo(pass.specularColor);
			pos.makeReferenceTo(pass.pos);
			center.makeReferenceTo(pass.center);
			biasScalar.makeReferenceTo(pass.biasScalar);
			isShading.makeReferenceTo(pass.isShading);
			isVolume.makeReferenceTo(pass.isVolume);
			isDrawSun.makeReferenceTo(pass.isDrawSun);
			isFrontCulling.makeReferenceTo(pass.isFrontCulling);
			scattering.makeReferenceTo(pass.scattering);
			volumeStrength.makeReferenceTo(pass.volumeStrength);
		}

		/*void disableLighting() { isLighting = false; }
		void enableLighting() { isLighting = true; }*/

	};

}
