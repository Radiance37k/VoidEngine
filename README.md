# VoidEngine
  
A game engine programmed in C++ utilizing Vulkan.
The ide is to have each part of the engine a seperate dll, only to be loaded if used to prevent the end result being bloated with unnecessary features. It should also make any patching of the engine easier since it is not the entire engine being updated, just the relevant dll.
  
This will be the intended class hierarchy. It is subject to change and it might be a bit off at the moment, it was made while I was still trying out Directx11. 
  
## Class hierarchy
```bash
Game ( VoidEngine.h )
   ├── CameraManager  ( Managers/CameraManager.h)
   │
   ├── InputManager  ( Managers/InputManager.h )
   │
   ├── ModelManager  ( Managers/ModelManager.h )
   │
   ├── RenderManager  ( Managers/RenderManager.h )
   │     ├── Device  
   │     ├── Context  
   │     ├── Swap Chain  
   │     ├── Render Target View  
   │     ├── Depth Buffer (Depth-Stencil Buffer)  
   │     ├── Constant buffer  
   │     ├── Rasterizer state  
   │     └── Blend state
   │
   ├── SceneManager  ( Managers/SceneManager.h )
   │     └── Scene  
   │
   ├── UIManager  ( Managers/UIManager.h )
   │  
   ├── GameObject  ( Core/GameObject.h )
   │     ├── Transform  
   │     └── Model  ( Components/Model.h )
   │           └── Mesh  ( Components/Mesh.h )
   │                  ├── Material  
   │                  ├── Texture  
   │                  └── Shaders
   │
   └── LightSourceManager ( Managers/LightSourceManager.h )
```
  
## Todo
A set of features I intend to implement:  
- [x] Load model  
- [x] Camera movement  
- [ ] Camera mouse movement  
- [ ] Input manager  
- [x] Lightsource  
- [ ] Bumpmapping  
- [ ] UI  
- [ ] Dynamic buffers  
- [ ] Dynamic lighting  
- [ ] Render queues  
  
## Wishlist
A set of features I'd like to implement:  
- [ ] Depth stencil    
- [ ] Render graph  
- [ ] Wireframe camera view frustum  
- [ ] Multiple cameras  
- [ ] Post-processing effects  
- [ ] Particle engine  
  
## SBOM
VulkanSDK  
glfw  
glm  
tinyobjloader, will be replaced with assimp
