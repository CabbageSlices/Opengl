#pragma once
#include "Enums.h"
#include "Includes.h"

class GLTextureObject;

class GLFramebufferObject {
  public:
    GLFramebufferObject();
    virtual ~GLFramebufferObject() { deleteFramebuffer(); }
    GLFramebufferObject(const GLFramebufferObject &rhs) = delete;
    GLFramebufferObject &operator=(const GLFramebufferObject &rhs) = delete;

    void create();

    void attachTexture(const FramebufferColorAttachment &attachment, const GLTextureObject &object,
                       unsigned int mipmapLevelInTextureToRenderTo, bool enableDrawingToThisAttachment = true);

    void attachTexture(const FramebufferNonColorAttachment &attachment, const GLTextureObject &object,
                       unsigned int mipmapLevelInTextureToRenderTo);

    void enableDrawingToAttachments(const std::vector<FramebufferColorAttachment> &attachments);
    void enableDrawingToAttachment(const FramebufferColorAttachment &attachment);

    void disableDrawingToAttachments();

    GLuint getFramebufferObject() { return framebufferObject; }

    void bindToTarget(const FramebufferTarget &target);

    void deleteFramebuffer() {
        if (framebufferObject == 0) return;

        glDeleteFramebuffers(1, &framebufferObject);
        framebufferObject = 0;
    }

    static void clearFramebufferAtTarget(const FramebufferTarget &target);

  private:
    void attachTexture(const GLenum &attachment, const GLTextureObject &object, unsigned int mipmapLevelInTextureToRenderTo);
    GLuint framebufferObject;
};