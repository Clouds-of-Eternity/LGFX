<p align="center">
  <img src="https://raw.githubusercontent.com/Clouds-of-Eternity/LGFX/eternity-works/Logo.gif" width="256"/>
</p>

# LGFX
> Third time's the charm

**LGFX** is an abstraction layer for Vulkan and perhaps other APIs in the future, aiming to be similar in many ways to WebGPU's API but with more freedom of control, including matters like synchronization.

LGFX itself is written in C, and does not come with any windowing library or input system. However, a higher-level helper API written in C++ is also provided, known as **AstralCanvas**. It uses GLFW and is meant to help users kickstart simple graphical apps by further bootstrapping functionality like instance and device creation, as well as providing an update, fixed update and render callback.

A rewrite for AstralCanvas to be in pure C but with C++ bindings is underway, but will probably not be completed soon due to the situation with ShaderCompiler and the shader format taking precedence.

While perhaps lower level than other interfaces, LGFX is *still* remarkably simple to use if you already know 'vanilla' Vulkan. A triangle takes less than 180 LOC.

## Building
1. Clone the repository, `cd` into the directory and run `git submodule update --init --recursive` to initialize all dependent submodules. For `Astral.Core`, remember to set it to the branch of the same name as this repository itself.
1. Install Premake5 ([If you don't already have it,](https://premake.github.io/)) as well as the Vulkan SDK if compiling for Vulkan and/or compiling ShaderCompiler.
2. cd into repo directory and run premake5. Options include `--clang` for use with the 'gmake' (Unix Makefiles) target, forcing the toolset to utilise clang instead of gcc. On Linux, specify `--x11` or `--wayland` for your preferred window manager.
3. Compile with the build system of your choice

## ShaderCompiler
Though the examples already include the compiled spir-v .func files, you can utilise the ShaderCompiler to compile new [Slang Shader Files](https://shader-slang.org/).

ShaderCompiler detects what type of shader a .slang file is by checking the name of the entry points. If it finds entry points named 'VertexFunction' and 'FragmentFunction,' it will treat the file as a vertex-fragment shader. If it finds an entry point named 'main,' it will simply be treated as a compute shader.

The ShaderCompiler CLI takes 1 necessary argument: The root directory containing all your .slang files. Thereafter, all arguments should be directory paths to include directories. 

ShaderCompiler treats all files with the `.inc.slang` extension as a utility file and not a complete shader on its own.

ShaderCompiler will eventually be moved into its own repository in a planned v2 rewrite, while its dependence on JSON as an output format will be replaced with something else.