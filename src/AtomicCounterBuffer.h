#pragma once

#include "ofVboMesh.h"
#include "ofBufferObject.h"
#include "ofGLProgrammableRenderer.h"

namespace ofxDeferred {
	class AtomicCounterBuffer {
	public:
		AtomicCounterBuffer() : maxCount(500) {
			// Create atomic counter buffer
			// and define its initial storage capacity
			glGenBuffers(1, &counterId);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
			glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

			// Indirect command
			DrawElementsIndirectCommand cmd{ 4, 0, 0, 0, 0 };

			// Create buffer storage for indirect buffer
			// Setup the indirect buffer
			glGenBuffers(1, &indirectBufferId);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &cmd, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

			// Create the texture proxy for the indirect buffer
			glGenTextures(1, &indirectBufferTex);
			glBindTexture(GL_TEXTURE_BUFFER, indirectBufferTex);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, indirectBufferId);
			glBindTexture(GL_TEXTURE_BUFFER, 0);

			synchIndirect.loadCompute(shaderPath + "bokeh/bokehSync.glsl");

		}

		~AtomicCounterBuffer() {
			glDeleteBuffers(1, &counterId);
			glDeleteBuffers(1, &indirectBufferId);
			glDeleteBuffers(1, &indirectBufferTex);
		}

		void bind() {
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterId);
			
			// Clear atomic counter buffer
			glm::uint32* bokehCounterValue = (glm::uint32*)glMapBufferRange(
				GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(glm::uint32),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
			);
			*bokehCounterValue = 0;
			glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
			
		}

		void setMaxCount(int maxCount) {
			this->maxCount = maxCount;
		}

		void unbind() {
			
			// Synch the atomic counter with the indirect texture for indirect instanced-rendering
			synchIndirect.begin();
			synchIndirect.setUniform1i("maxCount", maxCount);
			glActiveTexture(GL_TEXTURE0 + synchIndirect.getUniformLocation("indirectBufferTex"));
			glBindImageTexture(synchIndirect.getUniformLocation("indirectBufferTex"), indirectBufferTex, 0, false, 0, GL_WRITE_ONLY, GL_R32UI);
			synchIndirect.dispatchCompute(1, 1, 1);
			glActiveTexture(GL_TEXTURE0);
			synchIndirect.end();

			// unbind atomic counter buffer
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
			
		}

		void drawIndirect(ofVbo& vbo) {
			
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			
			vbo.bind();

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
			glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, NULL);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
				
			vbo.unbind();
		}

	private:
		struct DrawElementsIndirectCommand {
			GLuint  count;
			GLuint  primCount;
			GLuint  firstIndex;
			GLuint  baseVertex;
			GLuint  baseInstance;
		};

		// Atomic Counter Buffer ID
		GLuint counterId;

		// Texture buffer ID for indirect rendering
		GLuint indirectBufferId;
		GLuint indirectBufferTex;
		ofShader synchIndirect;
		int maxCount;
	};
}
