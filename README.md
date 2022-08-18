# GraphicsFinal

## Instructions
This project is ready to clone and run as is, except for two things:
1. The toolset used may be for Visual Studio 2019 (v142), so make sure to adjust the toolset to whatever you are using in the solution properties.
2. Run `Local Windows Debugger` build task (it will fail). Go to the `lib` folder, and copy `assimp-vc143-mtd.dll` (or `assimp-vc142-mtd.dll` if you are using Visual Studio 2019) to the newly created`x64/Debug`, this is the directory where the executable will be built. 

# Techniques

### Shadow Mapping
The castle and the grass in the scene cast shadows onto the terrain.

### Gamma Correction
We employed gamma correction to brighten the image a bit and make it pop more.

### Normal Mapping
The castle uses normal mapping to simulate lighting off of the rough texture.

### Blending
Blending was used on the water to aid in refraciton. You can enable blending on the grass in the IMGUI window.

### Framebuffers
We use 3 framebuffers:
1. Reflection buffer for the water reflection texture. We take the camera, invert y position and pitch, and use a clipping plane to render everything above the water.
2. Refraction buffer for the water refraction texture. We use a clipping plane to only render things below the water.
3. Depth buffer for shadow mapping

### Cube Map
There is a cubemap for a skybox in the background.

### Geometry Shader
The grass is done using a geometry shader on the points generated by the tessellation shader for the terrain. The geometry shader will not draw grass below water level, and it reads from a path texture to selectively not draw grass in some places as well to create a dirt path.

### Instancing
The tree models are drawn using instancing.

### Model Loading
The castle and the trees are loaded using Assimp.

### Tessellation Shader
The terrain is generated from a heightmap using a tessellation shader. For each patch generated by the tessellation shader, we compute the normal. Based on the normal, we either use a dirt texture or a cliff texture. W

