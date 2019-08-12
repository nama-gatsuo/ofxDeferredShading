#pragma once

#include "ofVboMesh.h"
#include "ofBufferObject.h"
#include "ofGLProgrammableRenderer.h"

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
				// Setup the indirect buffer
				glGenBuffers(1, &indirectBufferId);
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
				DrawElementsIndirectCommand cmd{ 4, 200, 0, 0, 0 };
				
				glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &cmd, GL_STATIC_DRAW);
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

				// Create the texture proxy for the indirect buffer
				glGenTextures(1, &indirectBufferTex);
				glBindBuffer(GL_TEXTURE_BUFFER, indirectBufferTex);
				glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, indirectBufferId);
				glBindTexture(GL_TEXTURE_BUFFER, 0);

				synchIndirect.loadCompute(shaderPath + "bokeh/bokehSync.glsl");

				testTex.allocate(1, 1, GL_R32F);
				testTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
			}

		}

		~AtomicCounterBuffer() {
			glDeleteBuffers(1, &counterId);
			if (isUseIndirectBuffer) {
				glDeleteBuffers(1, &indirectBufferId);
				glDeleteBuffers(1, &indirectBufferTex);
			}
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

		void unbind() {
			if (isUseIndirectBuffer) {
				// Synch the atomic counter with the indirect texture for indirect instanced-rendering
				/*synchIndirect.begin();
				synchIndirect.setUniform1i("maxCount", 500);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindImageTexture(1, indirectBufferTex, 0, false, 0, GL_WRITE_ONLY, GL_R32UI);	
				testTex.bindAsImage(2, GL_WRITE_ONLY);
				synchIndirect.dispatchCompute(1, 1, 1);
				glActiveTexture(GL_TEXTURE0);
				synchIndirect.end();
				testTex.unbind();*/
				

				// unbind atomic counter buffer
				glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
				
				//readFromAtomicCounterBuffer();

			} else {
				// unbind buffer base
				glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);

				readFromAtomicCounterBuffer();
			}

		}

		int getCount() { return count; }

		void drawIndirect(ofVboMesh& mesh) {
			auto& vbo = mesh.getVbo();

			if (isUseIndirectBuffer) {
				
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
								
				vbo.bind();

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
				glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, NULL);
				
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
				vbo.unbind();
				
			} else {
				
				vbo.bind();
				glDrawElementsInstanced(GL_TRIANGLE_STRIP, vbo.getNumIndices(), GL_UNSIGNED_INT, nullptr, count);
				vbo.unbind();
			
			}
			
		}

		const ofTexture& getTex() const {
			return testTex;
		}

	private:
		void readFromAtomicCounterBuffer() {
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
			{
				// Read from buffer
				glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &count);
				//ofLogNotice("Count") << count;

				// reset
				GLuint reset = 0;
				glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset);
			}
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		}


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
		//DrawElementsIndirectCommandDrawElementsIndirectCommand cmd;
		GLuint indirectBufferTex;
		const bool isUseIndirectBuffer;
		ofShader synchIndirect;


		ofTexture testTex;
	};
}
