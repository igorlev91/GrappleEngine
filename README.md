# Grapple Game Engine

## About
The project started as a way to learn how internal systems of a game engine work.

[Grapple Engine](https://youtu.be/O6Dmh0V-yYA)

[PostProcessing](https://youtu.be/jxzveqdi39A)

[ShadowMapping](https://youtu.be/1IYXRgtVCas)

[Atmosphere](https://youtu.be/PB9cLKihOjY)

[Decals](https://youtu.be/PCqHb4EIyvw)

## Rendering Features
 The 3D rendering is done by a forward renderer, which supports HRD, directional, point and spotlights. It also implements GPU instancing and frustum culling to reduce the number draw calls. There are 4 post processing effects available in total: Vignette, Tone mapping, Screen Space Ambient Occlusion, Atmosphere. 

Manually writing down all the pipeline barriers in Vulkan, is a rather error-prone process. In order to reduce the possibility of error and simplify the process of implementing new features into the rendering pipeline, I made a decision to implement a simple render graph, which is able to automatically generate pipeline barriers.

## Other features
Archetype based ECS. Allows for cache friendly access of entity components, by storing all entities of the same archetype sequentially in memory. Supports querying for existing entities and entities created or deleted during the frame. Includes support for adding system execution dependencies.

## Building

Run `/scripts/Windows_GenerateProjects.bat` to generate a Visual Studio 2022 solution.

### Dependecies

1. Vulkan SDK 1.3. `VulkanSDK/<version>/bin/` must be added to `PATH`. `VULKAN_SDK` environment variable must be set to `/VulkanSDK/<version>/`
2. Assimp is not included in the solution, and is built separatly as a static lib.
    Run `/Grapple/vendor/assimp/GenerateAssimpProject.bat` to generate a Visual Studio solution and build both Debug & Release configurations.
3. All other dependecies, are included as submodules and are a part of the Visual Studio solution.

### Sandbox

Run `GenerateProject.bat` inside `/Sandbox/`, to generate a solution.

## Running

The editor can be started using Visual Studio, or from the command line by running the `.exe` from the `GrappleEditor` folder.

### Command line arguments

`--project=<path>` - A project to open on editor startup. Must be a path to .Grappleproj file.

`--api=<name>` - API to use for rendering. Only `--api=vulkan` is supported, which is also a default.

`--vulkan-debug` - enables Vulkan validation layers and generation of debug names for Vulkan objects. Disabled by default.

`--device=<type>` - specify a type of GPU device to use for rendering. Can be one of two: `integrated` or `discrete`. `discrete` is the default.
