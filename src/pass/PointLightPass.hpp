#pragma once
#include "RenderPass.hpp"
#include "ofVboMesh.h"
#include "ofTypes.h"

namespace ofxDeferred {
	class PointLight {
	public:
		PointLight(const std::string& name) {
			lightNum++;
			group.setName(name);
			group.add(isActive.set("isActive", true));
			group.add(diffuseColor.set("diffuse_color", ofFloatColor(.4)));
			group.add(specularColor.set("specular_color", ofFloatColor(.9)));
			group.add(position.set("position", glm::vec3(), glm::vec3(-800.), glm::vec3(800.)));
			group.add(radius.set("radius", 100., 0.1, 600.));
			group.add(intensity.set("intensity", 0.5, 0., 1.));
		}
		virtual ~PointLight() {
			lightNum--;
		}

		void setRandomParameters() {
			diffuseColor.set(getRandomColor(ofFloatColor(0.2), ofFloatColor(0.9)));
			specularColor.set(getRandomColor(ofFloatColor(0.4), ofFloatColor(1.)));
			position.set(getRandomVec(position));
			radius.set(ofRandom(radius.getMin(), radius.getMax()));
			intensity.set(ofRandom(intensity.getMin(), intensity.getMax()));	
		}

		ofParameterGroup& getParameters() { return group; }
		
		ofParameter<bool> isActive;
		ofParameter<ofFloatColor> diffuseColor;
		ofParameter<ofFloatColor> specularColor;
		ofParameter<glm::vec3> position;
		ofParameter<float> intensity;
		ofParameter<float> radius;
	private:
		ofParameterGroup group;

		static int lightNum;
		static inline ofFloatColor getRandomColor(const ofFloatColor& min, const ofFloatColor& max) {
			return ofFloatColor(
				ofRandom(min.r, max.r),
				ofRandom(min.g, max.g),
				ofRandom(min.b, max.b)
			);
		}
		static inline glm::vec3 getRandomVec(const glm::vec3& min, const glm::vec3& max) {
			return glm::vec3(
				ofRandom(min.r, max.r),
				ofRandom(min.g, max.g),
				ofRandom(min.b, max.b)
			);
		}
		static inline glm::vec3 getRandomVec(const ofParameter<glm::vec3>& vp) {
			return getRandomVec(vp.getMin(), vp.getMax());
		}
	};

	class PointLightPass : public RenderPass {
	private:
		std::vector<ofPtr<PointLight>> lights;
		ofShader shader;
		glm::mat4 modelViewMatrix;
		ofVboMesh sphere;
		ofShader lightShader;
		ofParameter<float> lightBrightness;
	public:
		using Ptr = std::shared_ptr<PointLightPass>;
		PointLightPass(const glm::vec2& size);

		ofPtr<PointLight> addLight() {
			auto pl = std::make_shared<PointLight>("light_" + ofToString(lights.size()));
			pl->setRandomParameters();
			return addLight(pl);
		}
		ofPtr<PointLight> addLight(ofPtr<PointLight> pl) {
			group.add(pl->getParameters());
			lights.push_back(pl);
			return pl;
		}
		ofPtr<PointLight> getLight(int index) {
			return lights[index];
		}
		void clear() { lights.clear(); }
		size_t getLightsSize() const { return lights.size(); }
		std::vector<ofPtr<PointLight>>& getLights() { return lights; }
		
		void update(const ofCamera& cam) override;
		void render(const ofTexture& read, ofFbo& write, const GBuffer& gbuffer) override;
		void drawLights(ofPolyRenderMode mode = OF_MESH_FILL);
		void drawLights(const RenderInfo& info);
		
		void setColorBrightness(float birghtness) {
			lightBrightness = birghtness;
		}

		void refer(PointLightPass& pass) {
			//enabled.makeReferenceTo(pass.enabled);
			//lightBrightness.makeReferenceTo(pass.lightBrightness);
			for (auto light : pass.lights) {
				addLight(light);
			}
		}
	};
}
