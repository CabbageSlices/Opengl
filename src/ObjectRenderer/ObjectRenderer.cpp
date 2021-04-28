#include "./ObjectRenderer.h"

#include "RenderBlueprint/RenderBlueprint.h"

void ObjectRenderer::setShaderForPass(const RenderingPass &pass, const std::shared_ptr<ShaderProgram> &program) {
    // shaderForEachPass[pass] = program;
}

void ObjectRenderer::render() { renderInternal(); }
