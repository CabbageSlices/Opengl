#ifndef __OBJECTRENDERER_H__
#define __OBJECTRENDERER_H__

#include <map>
#include <memory>

#include "GraphicsWrapper.h"
#include "ShaderProgram/ShaderProgram.h"

class RenderBlueprint;

// handles rendering behaviour for an object
class ObjectRenderer {
  public:
    virtual ~ObjectRenderer() {}
    void setShaderForPass(const RenderingPass &pass, const std::shared_ptr<ShaderProgram> &program);
    void render();

  protected:
    virtual void renderInternal() = 0;
};
#endif  // __OBJECTRENDERER_H__