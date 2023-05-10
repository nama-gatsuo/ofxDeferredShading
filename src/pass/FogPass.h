#pragma once
#include "RenderPass.hpp"
#include "ofMath.h"

namespace ofxDeferred {
	class FogPass : public RenderPass {
	public:

		FogPass(const glm::vec2& size) : RenderPass(size, RenderPassRegistry::Fog),
			t(0.f),
			farClip(0),
			nearClip(0),
			fovy(0)
		{
			shader.load(passThruPath, shaderPath + "fog.frag");

			group.add(noiseFogStrength.set("noiseFogStrength", 1.f, 0.f, 1.f));
			group.add(depthFogStrength.set("depthFogStrength", 1.f, 0.f, 1.f));
			group.add(fogColor.set("fogColor", ofFloatColor(0.85f, 0.85f, 0.82f)));
			group.add(fogAmp.set("fogAmp", 0.2f, 0.001f, 1.f));
			group.add(fogFreq.set("fogFreq", glm::vec3(4.f), glm::vec3(0.1f), glm::vec3(10.f)));
			group.add(fogOffset.set("fogOffset", glm::vec3(0.4f), glm::vec3(-10.f), glm::vec3(10.f)));
			group.add(fogCenter.set("fogCenter", 0.5f, 0.f, 1.f));
			group.add(fogPower.set("fogPower", 1.f, 0.1f, 2.f));
			group.add(fogRayStart.set("fogRayStart", 400.f, 0.1f, 3000.f));
			
		}

		void update(const ofCamera& cam) override {
			camTransform = cam.getGlobalTransformMatrix();
			fovy = ofDegToRad(cam.getFov());
			lensOffset = cam.getLensOffset();
			farClip = cam.getFarClip();
			nearClip = cam.getNearClip();
			t += 0.01f;
		}

		void render(const ofTexture& read, ofFbo& writeFbo, const ofxDeferred::GBuffer& gbuffer) override {

			writeFbo.begin();
			ofClear(0);

			shader.begin();
			shader.setUniform2f("res", size);
			shader.setUniformMatrix4f("camModelMatrix", camTransform);
			shader.setUniform3f("camPos", camTransform * glm::vec4(0, 0, 0, 1));
			shader.setUniform1f("fovy", fovy);
			shader.setUniform1f("farClip", farClip);
			shader.setUniform1f("nearClip", nearClip);
			shader.setUniform1f("lds", 1.f / (farClip - nearClip));
			shader.setUniform2f("lensOffset", lensOffset);
			shader.setUniformTexture("depth", gbuffer.getTexture(ofxDeferred::GBuffer::TYPE_DEPTH_NORMAL), 1);
			shader.setUniform1f("time", t);
			shader.setUniforms(group);
			shader.setUniform4f(fogColor.getName(), fogColor);
			read.draw(0, 0);
			shader.end();

			writeFbo.end();
		}
		
		void setColor(const ofFloatColor& c) { fogColor.set(c); }
		void setNoiseFogDensity(float f) { noiseFogStrength.set(f); }
		void setDepthFogDensity(float g) { depthFogStrength.set(g); }

		void refer(FogPass& pass) {
			enabled.makeReferenceTo(pass.enabled);
			noiseFogStrength.makeReferenceTo(pass.noiseFogStrength);
			depthFogStrength.makeReferenceTo(pass.depthFogStrength);
			fogColor.makeReferenceTo(pass.fogColor);
			fogAmp.makeReferenceTo(pass.fogAmp);
			fogFreq.makeReferenceTo(pass.fogFreq);
			fogOffset.makeReferenceTo(pass.fogOffset);
			fogCenter.makeReferenceTo(pass.fogCenter);
			fogPower.makeReferenceTo(pass.fogPower);
			fogRayStart.makeReferenceTo(pass.fogRayStart);
		}

	private:
		ofShader shader;
		glm::vec2 lensOffset;
		glm::mat4 camTransform;
		float fovy;
		float farClip, nearClip;
		ofParameter<float> noiseFogStrength;
		ofParameter<float> depthFogStrength;
		ofParameter<ofFloatColor> fogColor;
		ofParameter<float> fogAmp;
		ofParameter<glm::vec3> fogFreq;
		ofParameter<glm::vec3> fogOffset;
		ofParameter<float> fogCenter;
		ofParameter<float> fogPower;
		ofParameter<float> fogRayStart;
		float t;
	};

};
