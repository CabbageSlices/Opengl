#include "GLFramebufferObject.h"

#include "GLTextureObject.h"

void GLFramebufferObject::clearFramebufferAtTarget(const FramebufferTarget &target) { glBindFramebuffer(target, 0); }

GLFramebufferObject::GLFramebufferObject() : framebufferObject(0) {}

void GLFramebufferObject::create() {
    if (framebufferObject == 0) {
        glCreateFramebuffers(1, &framebufferObject);
    }
}

void GLFramebufferObject::attachTexture(const FramebufferColorAttachment &attachment, const GLTextureObject &object,
                                        unsigned int mipmapLevelInTextureToRenderTo, bool enableDrawingToThisAttachment) {
    attachTexture((GLenum)attachment, object, mipmapLevelInTextureToRenderTo);

    if (enableDrawingToThisAttachment) {
        enableDrawingToAttachment(attachment);
    }
}

void GLFramebufferObject::attachTexture(const FramebufferNonColorAttachment &attachment, const GLTextureObject &object,
                                        unsigned int mipmapLevelInTextureToRenderTo) {
    attachTexture((GLenum)attachment, object, mipmapLevelInTextureToRenderTo);
}

void GLFramebufferObject::enableDrawingToAttachments(const std::vector<FramebufferColorAttachment> &attachments) {
    if (framebufferObject == 0) {
        throw "No framebuffer present";
    }
    glNamedFramebufferDrawBuffers(framebufferObject, attachments.size(), (GLenum *)attachments.data());
}
void GLFramebufferObject::enableDrawingToAttachment(const FramebufferColorAttachment &attachment) {
    if (framebufferObject == 0) {
        throw "No framebuffer present";
    }

    glNamedFramebufferDrawBuffer(framebufferObject, attachment);
}

void GLFramebufferObject::bindToTarget(const FramebufferTarget &target) { glBindFramebuffer(target, framebufferObject); }

void GLFramebufferObject::attachTexture(const GLenum &attachment, const GLTextureObject &object,
                                        unsigned int mipmapLevelInTextureToRenderTo) {
    if (framebufferObject == 0) {
        create();
    }

    glNamedFramebufferTexture(framebufferObject, attachment, object.getTextureObject(), mipmapLevelInTextureToRenderTo);
}
