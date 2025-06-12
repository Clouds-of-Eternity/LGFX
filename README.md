<p align="center">
  <img src="https://raw.githubusercontent.com/Clouds-of-Eternity/LGFX/eternity-works/Logo.gif" width="256"/>
</p>

# LGFX
> Third time's the charm

LGFX (Linx GFX) is a low-level abstraction layer for Vulkan, aiming to be similar in many ways to WebGPU's API but with more freedom of control, including things like synchronization.

A pure C API with optional helper structures written in C++, LGFX was created to meet the growing demands of my game engine, in which the regular high-level abstraction of Astral.Canvas could no longer meet.

Despite this, LGFX is *still* remarkably simple to use if you already know 'vanilla' Vulkan. A triangle takes less than 180 LOC! 

By itself, it does not come with any windowing library or input system. However, the higher-level helper API (lgfx-astral) uses GLFW and is meant to help users kickstart simple graphical apps by further bootstrapping trivial matters like instance and device creation and providing a render loop.

## Building
1. Install Premake5 ([If you don't already have it,](https://premake.github.io/)) as well as the Vulkan SDK if compiling for Vulkan and/or compiling ShaderCompiler.
2. cd into repo directory, run premake5. Options include `--clang` for use with the 'gmake' target, forcing the toolset to utilise clang instead of gcc. On Linux, specify `--x11` or `--wayland` for your preferred window manager.
3. Compile with the build system of your choice

## ShaderCompiler
Though the examples already include the compiled spir-v .func files, you can utilise the ShaderCompiler to compile new [Slang Shader Files](https://shader-slang.org/).

ShaderCompiler detects what type of shader a .slang file is by checking the name of the entry points. If it finds entry points named 'VertexFunction' and 'FragmentFunction,' it will treat the file as a vertex-fragment shader. If it finds an entry point named 'main,' it will simply be treated as a compute shader.

The ShaderCompiler CLI takes 1 necessary argument: The root directory containing all your .slang files. Thereafter, all arguments should be directory paths to include directories. 

ShaderCompiler treats all files with the `.inc.slang` extension as a utility file and not a complete shader on its own.