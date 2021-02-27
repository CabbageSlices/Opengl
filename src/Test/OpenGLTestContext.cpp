#include "./OpenGLTestContext.h"

using std::make_shared;

shared_ptr<OpenGLTestContext> OpenGLTestContext::testContext;

OpenGLTestContext::OpenGLTestContext() : usingRealOpenGL(false) {}

shared_ptr<OpenGLTestContext> OpenGLTestContext::getContext() {
    if (!OpenGLTestContext::testContext) {
        createContext();
    }

    return OpenGLTestContext::testContext;
}

void OpenGLTestContext::createBuffers(GLsizei n, GLuint *buffers) {
    // to do
}

void OpenGLTestContext::useRealOpenGL() { usingRealOpenGL = true; }
void OpenGLTestContext::createContext() { testContext = shared_ptr<OpenGLTestContext>(new OpenGLTestContext()); }
void OpenGLTestContext::deleteContext() { testContext.reset(); }