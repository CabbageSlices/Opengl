#include "Enums.h"
#include "glad\glad.h"

class Buffer;

/*
    Buffer offset/stride & attribute offset/size explanation:
    suppose the buffer contains data as follows
    FILLER DATA (1024 bytes), Position (vec4), Normal (vec3), position, normal, position

    then the buffer offset = 1024, address of first element used for attribute data
    stride = sizeof(vec4) + sizeof(vec3), basically the distance from one data element for a given attribute to the next
    so from one position dataa to the next position data there is a stride of vec4 + vec3

    For attribute format we have:
    position num elements = 4, offset = 0.
    normal num elements = 3, offset = sizeof(vec4)

    attribute offset is the distance WITHIN the porition of the buffer that is used for attribute data
    this is measured from the position of the FIRST element in the buffer usesd for ATTRIBTUE data
    NOT THE BEGINNING OF THE BUFFER ITSELF

    num elements determines how many bytes of data to pass for each element of a given attribute
    for position there will be 4 * sizeof(data type used)
    for normal there will be 3 * sizeof(data type used)


    position starts from the beginning of the data region so it has no offset.
    normal comes after position so it has offset sizeof(vec4)

    the 2nd position data position is computed using buffer stride
    = data_position of first position element + buffer stride
    number of bytes corresponding to each
 */
class VertexArrayObject {
  public:
    VertexArrayObject();

    ~VertexArrayObject() { deleteVao(); }

    // copying  vertex array objects don't make sense so delete them
    VertexArrayObject(const VertexArrayObject &rhs) = delete;
    VertexArrayObject &operator=(const VertexArrayObject &rhs) = delete;

    /**
     * @brief Connects the vertex attribute located aat shaderLocation within the shader program to the
     * buffer bound at bufferBindingIndex. DOES NOT BIND BUFFER TO INDEX, buffer must be bound to the index as element array
     * buffer for the data to reach the shader. need to call bindVertexAttributeBuffer
     * Basically says, whatever buffer i decide to bind to this binding index will be responsible for sending data to
     * the attribute at the given shader Location. Need to actually bind the buffer with bindVertexAttributeBuffer in order
     * to the data to be sent.
     *
     * @param shaderLocation location in shader th attribute data is located
     * @param bufferBindingIndex binding index of the buffer where dataa will be read from
     * @param enableeAttribute should the attribute be enabled? can also call enableVertexAttribute manually. Note, if
     * attribute is already enabled then this function will NOT disable it
     */
    void vertexAttributeShaderLocationToBufferIndex(GLuint shaderLocation, GLuint bufferBindingIndex,
                                                    bool enableAttribute = true);

    // connect the given index buffer to the vertex aarray object, that way when this vertex array object is bound to the
    // context the attatched buffer is bound as well
    void attachElementBuffer(const Buffer &elementArrayBuffer);

    /**
     * @brief binds the given buffer to the binding index so that shaders will receive the data as vertex input.
     * the bindingIndex must be connected to a shader attribute location using vertexAttributeShaderLocationToBufferIndex().
     *
     * @param bufferBindingIndex location to bind the buffer
     * @param buffer buffer where data will be reaad from
     * @param offset offset into the buffer where the data begins. in Bytes
     * @param stride distance between each item in the buffer, in bytes.
     */
    void bindVertexAttributeBuffer(GLuint bufferBindingIndex, const Buffer &buffer, GLintptr offset, GLsizei stride);

    /**
     * @brief specifies the format of the data being passed to the vertex shader at the given attribtue location
     * must be called before drawing
     *
     * @param shaderAttributeLocation location of the attribute in the vertex shader
     * @param numElementsPerEntry number of elements in each entry. if the attribtue is a vec4 then number of elements is 4
     * @param dataType underlying data type of the attribute,
     * @param offset offset to the first element of the attribute, see class description for full explanation
     */
    void formatVertexAttributeData(GLuint shaderAttributeLocation, GLuint numElementsPerEntry, DataType dataType,
                                   GLuint offset = 0, bool normalize = false);

    void enableVertexAttribute(GLuint shaderAttributeLocation);

    void bindToContext();
    void unbindFromContext();

    void deleteVao() {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    GLuint getVao() const { return vao; }

  private:
    void init();

    GLuint vao;
};