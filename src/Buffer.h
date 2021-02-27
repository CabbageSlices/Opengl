#pragma once
#include "GraphicsWrapper.h"

class Buffer {
  public:
    enum BufferType : GLenum {
        None = GL_ZERO,
        UniformBuffer = GL_UNIFORM_BUFFER,
        ArrayBuffer = GL_ARRAY_BUFFER,
        ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER
    };

    enum UsageType : GLenum {

        StaticDraw = GL_STATIC_DRAW,
        StreamDraw = GL_STREAM_DRAW
    };

    Buffer() : buffer(0), bufferType(None) {}

    ~Buffer() { deleteBuffer(); }

    Buffer(const Buffer &rhs) = delete;
    Buffer &operator=(const Buffer &rhs) = delete;

    void deleteBuffer() {
        if (buffer == 0) return;

        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }

    /**
     * @brief Create the buffer object and store the given data in the buffer
     * does not bind the buffer to the context. will replace existing data with new data if a buffer already exists
     *
     * @param target the buffer target that the buffer should be bound
     * @param pointerToData pointer to data to store in the buffer
     * @param bufferSize how big the buffer should be, in bytes
     * @param usageType how the buffer will be sued
     * @return true if the buffer is created successfully
     * @return false otherewise
     */
    bool create(const BufferType &type, const void *pointerToData, GLsizeiptr bufferSize, const UsageType &usageType);
    bool updateData(const void *pointerToData, GLsizeiptr dataSize, GLintptr offsetIntoBuffer);

    bool isUsed() { return buffer != 0; }

    /**
     * @brief Bind to the buffer type to set the active buffer for that binding target to this buffer, so opengl uses it for
     * any subsequent operation that reads from this binding target
     *
     */
    void bindToTarget();

    /**
     * @brief Clears the buffer currently bound to the target
     *
     */
    void unbindFromTarget();

    /**
     * @brief bind the given buffer to the given index in the list of targets stored by the type of this buffer.
     * uniform buffer's and the like have a list of binding targets for buffers to bind to in order for shaders to access
     * them the binding index is the index inside that array to bind this buffer to. Shader programs automatically have
     * access to buffers bound to this array, that way multiple shader programs can access the same data without the use of
     * glSetUniform
     */
    void bindToTargetBindingPoint(int bindingIndex);

    GLuint getBufferObject() const { return buffer; }

  private:
    GLuint buffer;
    BufferType bufferType;
};