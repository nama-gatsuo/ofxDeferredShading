#pragma once
#include "Processor.hpp"
#include "ofVboMesh.h"

namespace ofxDeferred {
	struct PointLight {
		ofFloatColor diffuseColor;
		ofFloatColor specularColor;
		glm::vec3 position;
		float intensity = 1.f;
		float radius = 50.f;
	};

	class PointLightPass : public RenderPass {
	protected:
		std::vector<PointLight> lights;
		ofShader shader;
		glm::mat4 modelViewMatrix;
		ofVboMesh sphere;
		ofShader lightShader;

	public:
		using Ptr = std::shared_ptr<PointLightPass>;
		PointLightPass(const glm::vec2& size);

		void addLight(PointLight light) {
			lights.push_back(light);
		}
		PointLight& getLightRef(int index) {
			return lights[index];
		}
		void clear() { lights.clear(); }
		int getLightsSize() const { return lights.size(); }
		std::vector<PointLight>& getLights() { return lights; }
		void update(const ofCamera& cam);
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void drawLights(ofPolyRenderMode mode = OF_MESH_FILL);
		void drawLights(ofCamera& cam, bool isShadow = false, ofPolyRenderMode mode = OF_MESH_FILL);
	};
}
