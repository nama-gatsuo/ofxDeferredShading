#pragma once

#include "ofVboMesh.h"
#include "ofBufferObject.h"

namespace ofxDeferred {
	class AtomicCounterBuffer {
	public:
		AtomicCounterBuffer(bool bUseIndirectBuffer = false) : isUseIndirectBuffer(bUseIndirectBuffer), count(0) {
			// Create atomic counter buffer
			glGenBuffers(1, &counterId);

			// bind the buffer and define its initial storage capacity
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
			glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

			if (isUseIndirectBuffer) {

				// Create buffer storage for indirect buffer
				
				glGenBuffers(1, &indirectBufferId);
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
				DrawElementsIndirectCommand cmd{ 1, 4, 0, 0, 0 };
				glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &cmd, GL_DYNAMIC_DRAW);

				glGenTextures(1, &texId);
				glBindBuffer(GL_TEXTURE_BUFFER, texId);
				glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, indirectBufferId);
				glBindTexture(GL_TEXTURE_BUFFER, 0);

				synchIndirect.loadCompute(shaderPath + "bokeh/bokehSync.glsl");
			}

		}

		~AtomicCounterBuffer() {
			glDeleteBuffers(1, &counterId);
			if (isUseIndirectBuffer) {
				glDeleteBuffers(1, &texId);
			}
		}

		void bind() {
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterId);
		}

		void unbind() {

			if (isUseIndirectBuffer) {
				// Synch the atomic counter with the indirect texture for indirect instanced-rendering
				synchIndirect.begin();
				synchIndirect.setUniform1i("maxCount", 500);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindImageTexture(1, texId, 0, false, 0, GL_WRITE_ONLY, GL_R32UI);
				synchIndirect.dispatchCompute(1, 1, 1);
				glActiveTexture(GL_TEXTURE0);
				synchIndirect.end();

				// Clear atomic counter buffer
				glm::uint32* bokehCounterValue = (glm::uint32*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(glm::uint32), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
				*bokehCounterValue = 0;
				glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

				// unbind atomic counter buffer
				glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			} else {
				// unbind buffer base
				glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);

				glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
				{
					// Read from buffer
					glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &count);

					// reset
					GLuint reset = 0;
					glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset);
				}
				glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
			}

		}

		int getCount() { return count; }

		void drawIndirect(ofVboMesh& vbo) {
			
			if (isUseIndirectBuffer) {
				glBindVertexArray(vbo.getVbo().getVaoId());
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, texId);

				glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, NULL);
				//glDrawArraysIndirect(GL_TRIANGLE_STRIP, NULL);

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

				glBindVertexArray(0);
			} else {
				vbo.drawInstanced(OF_MESH_FILL, count);
			}
			
		}

	private:
		struct DrawArraysIndirectBufferCommand {
			GLuint count;
			GLuint primCount;
			GLuint first;
			GLuint reservedMustBeZero;
		};

		struct DrawElementsIndirectCommand {
			GLuint  count;
			GLuint  primCount;
			GLuint  firstIndex;
			GLuint  baseVertex;
			GLuint  baseInstance;
		};

		// Atomic Counter Buffer ID
		GLuint counterId;
		unsigned int count;

		// Texture buffer ID for indirect rendering
		GLuint indirectBufferId;
		GLuint texId;
		const bool isUseIndirectBuffer;
		ofShader synchIndirect;
	};
}
