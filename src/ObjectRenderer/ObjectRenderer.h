#ifndef __OBJECTRENDERER_H__
#define __OBJECTRENDERER_H__

#include <map>
#include <memory>

#include "GraphicsWrapper.h"
#include "ShaderProgram/ShaderProgram.h"

// handles rendering behaviour for an object
// keeps track of what shader the object should use for each pass
class ObjectRenderer {
  public:
    ObjectRenderer(const std::map<RenderingPass, std::shared_ptr<ShaderProgram> > &shaderForPasses);
    virtual ~ObjectRenderer() {}
    void setShaderForPass(const RenderingPass &pass, const std::shared_ptr<ShaderProgram> &program);
    void render();

  protected:
    virtual void renderInternal() = 0;
    std::map<RenderingPass, std::shared_ptr<ShaderProgram> > shaderForEachPass;
};
#endif  // __OBJECTRENDERER_H__