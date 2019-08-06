#pragma once

#include "ofBufferObject.h"

class AtomicCounterBuffer {
public:
	AtomicCounterBuffer() : count(0) {
		// Create atomic counter buffer
		glGenBuffers(1, &counterId);

		// bind the buffer and define its initial storage capacity
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
			
		// Create indirect buffer
		/*GLuint indirectBufferId;
		glGenBuffers(1, &indirectBufferId);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferId);
		DrawArraysIndirectBufferCommand cmd{ 1, 0, 0, 0 };
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectBufferCommand), &cmd, GL_DYNAMIC_DRAW);
		
		glGenTextures(1, &texId);
		glBindBuffer(GL_TEXTURE_BUFFER, texId);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, indirectBufferId);*/
		
	}

	void bind() {
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterId);
	}

	void unbind() {
		// unbind buffer base
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterId);
		
		// Read from buffer
		glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &count);
		
		// reset
		GLuint reset = 0;
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	}

	int getCount() { return count; }

private:
	struct DrawArraysIndirectBufferCommand {
		GLuint count;
		GLuint primCount;
		GLuint first;
		GLuint reservedMustBeZero;
	};

	GLuint counterId;
	GLuint texId;
	unsigned int count;
	
};