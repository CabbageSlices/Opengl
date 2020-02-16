#pragma once
#include "glad\glad.h"

class Buffer {

public:

	enum BindingTarget : GLuint {
			None = GL_ZERO,
			UniformBuffer = GL_UNIFORM_BUFFER,
			ArrayBuffer = GL_ARRAY_BUFFER,
			ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER
	};

	enum UsageType : GLuint {
			
			StaticDraw = GL_STATIC_DRAW,
			StreamDraw = GL_STREAM_DRAW
	};

	Buffer(): buffer(0), bindingTarget(None) {}

	~Buffer() {
			deleteBuffer();
	}

	void deleteBuffer() {
			if(buffer == 0)
					return;
					
			unbindFromTarget();
			glDeleteBuffers(1, &buffer);
	}

	/**
	 * @brief Create the buffer object and store the given data in the buffer
	 * does not bind the buffer to the context. Fails if this object has already created a buffer
	 * and has not deleted it.
	 * @param target the buffer target that the buffer should be bound 
	 * @param pointerToData pointer to data to store in the buffer
	 * @param bufferSize how big the buffer should be, in bytes
	 * @param usageType how the buffer will be sued
	 * @return true if the buffer is created successfully
	 * @return false otherewise
	 */
	bool create(const BindingTarget &target, const void * pointerToData, GLsizeiptr bufferSize, const UsageType &usageType);
	bool updateData(const void *pointerToData, GLsizeiptr dataSize, GLintptr offsetIntoBuffer);

	bool isUsed() {
			return buffer != 0;
	}
	
	/**
	 * @brief Bind to the buffer type to tell opengl what kind of buffer this is (i.e uniform buffer, array buffer, etc)
	 * 
	 */
	void bindToTarget();

	void unbindFromTarget();

	/**
	 * @brief bind the given buffer to the given index in the list of targets stored by the type of this buffer
	 * uniform buffer's and the like have a list of binding targets for buffers to bind to in order for shaders to access them
	 * the binding index is the index inside that array to bind this buffer to
	 */
	void bindToTargetBindingPoint(int bindingIndex);

	GLuint getBufferObject() const {
			return buffer;
	}

private:

	GLuint buffer;
	BindingTarget bindingTarget;
};