#pragma once
#include "Includes.h"
#include "glad/glad.h"

using std::shared_ptr;
using std::string;
using std::weak_ptr;

// class to store my own opengl context
// very basic, only stores data that i really need
class OpenGLTestContext {
  public:
    static bool doesTestContextExist() { return !!testContext; }
    static shared_ptr<OpenGLTestContext> getContext();

    static void createContext();
    static void deleteContext();

    template <typename T>
    OpenGLTestContext createPropertyForUniformBlock(string propertyName) {
        // something like
        unsigned sizeOfProperty = sizeof(T);
        return testContext;
    }

    void useRealOpenGL();

    bool usingRealOpenGL;

    void createBuffers(GLsizei n, GLuint *buffers);

  private:
    OpenGLTestContext();

    static shared_ptr<OpenGLTestContext> testContext;
};