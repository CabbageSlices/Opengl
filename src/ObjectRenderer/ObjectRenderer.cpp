#include "./ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(const std::map<RenderingPass, std::shared_ptr<ShaderProgram> > &shaderForPasses)
    : shaderForEachPass(shaderForPasses) {}

void ObjectRenderer::setShaderForPass(const RenderingPass &pass, const std::shared_ptr<ShaderProgram> &program) {
    shaderForEachPass[pass] = program;
}

void ObjectRenderer::render() {
    // no shader for this pass so don't render anything
    if (shaderForEachPass.count(currentRenderingPass) == 0) {
        return;
    }

    shaderForEachPass[currentRenderingPass]->useProgram();

    renderInternal();
}
