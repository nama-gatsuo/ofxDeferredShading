#pragma once
#include "Processor.hpp"
#include "ofVboMesh.h"

namespace ofxDeferred {
	struct PointLight {
		ofParameter<ofFloatColor> diffuseColor;
		ofParameter<ofFloatColor> specularColor;
		ofParameter<glm::vec3> position;
		ofParameter<float> intensity;
		ofParameter<float> radius;
		ofParameterGroup group;

		void setParameters() {
			group.add(diffuseColor.set("diffuse_color", ofFloatColor(.4)));
			group.add(specularColor.set("specular_color", ofFloatColor(.9)));
			group.add(position.set("position", glm::vec3(), glm::vec3(-1000.), glm::vec3(1000.)));
			group.add(radius.set("radius", 100., 0.1, 1500.));
			group.add(intensity.set("intensity", 0.5, 0., 1.));
		}
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

		PointLight& addLight() {
			PointLight& pl = addLight(PointLight());
			pl.diffuseColor.set(getRandomColor(ofFloatColor(0.2), ofFloatColor(0.9)));
			pl.specularColor.set(getRandomColor(ofFloatColor(0.4), ofFloatColor(1.)));
			pl.position.set(getRandomVec(pl.position));
			pl.radius.set(ofRandom(pl.radius.getMin(), pl.radius.getMax()));
			pl.intensity.set(ofRandom(pl.intensity.getMin(), pl.intensity.getMax()));
			return pl;
		}
		PointLight& addLight(const PointLight& light) {
			lights.push_back(light);
			PointLight& pl = lights[lights.size() - 1];
			pl.group.setName("lighst_" + ofToString(lights.size()));
			pl.setParameters();
			group.add(pl.group);
			return pl;
		}
		PointLight& getLight(int index) {
			return lights[index];
		}
		void clear() { lights.clear(); }
		int getLightsSize() const { return lights.size(); }
		std::vector<PointLight>& getLights() { return lights; }
		
		void update(const ofCamera& cam);
		void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);

		void drawLights(ofPolyRenderMode mode = OF_MESH_FILL);
		void drawLights(ofCamera& cam, bool isShadow = false, ofPolyRenderMode mode = OF_MESH_FILL);
		
		inline ofFloatColor getRandomColor(const ofFloatColor& min, const ofFloatColor& max) {
			return ofFloatColor(
				ofRandom(min.r, max.r),
				ofRandom(min.g, max.g),
				ofRandom(min.b, max.b)
			);
		}
		inline glm::vec3 getRandomVec(const glm::vec3& min, const glm::vec3& max) {
			return glm::vec3(
				ofRandom(min.r, max.r),
				ofRandom(min.g, max.g),
				ofRandom(min.b, max.b)
			);
		}
		inline glm::vec3 getRandomVec(const ofParameter<glm::vec3>& vp) {
			return getRandomVec(vp.getMin(), vp.getMax());
		}
	};
}
