# Opengl
Rendering engine made with C++ and OpenGL. Created to use as a testbed for various rendering effects.
Currently loads and renders a mesh from a .obj file with support for textures, point lights, direcitonal lights, and shadows (both point and directional).
Uses a custom blueprint json schema to specify material properties and shaders, similar to unity's shaderlab code.

##### Example Blueprint
``` JSON
{
  "materials": ["DiffuseMaterial"],
  "shaderPrograms": {
    "shader1": {
      "vertex": "shadowMapPass.vert",
      "geometry": "shadowMapPass.geom",
      "fragment": "shadowMapPass.frag"
    },
    "shader2": {
      "vertex": "vertex.vert",
      "fragment": "fragment.frag"
    },
  },
  "passes": {
    "DEPTH_PASS": {
      "shaderProgram": "shader1"
    },
    "REGULAR_PASS": {
      "shaderProgram": "shader2",
      "materials": ["DiffuseMaterial"]
    }
  }
}

```

##### Example Scene
![image](https://user-images.githubusercontent.com/8029975/132937382-2da301a8-16e4-462a-a8e1-bfb2e6d58cf6.png)


Planned to add in the future:

- Deferred rendering
- Forward+ rendering
- multi render pass specification via blueprint
- expand schema to allow material definition, uniform definitions
- allow defining shaders within blueprint itself, similar to shaderlab, with automatic definitions for uniforms
- Global illumination
