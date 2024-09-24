# LGFX
> Third time's the charm

LGFX (Linx GFX) is a low-level abstraction layer for Vulkan, aiming to be similar in many ways to WebGPU's API but with more freedom of control, including things like synchronization.

A pure C API with optional helper structures written in C++, LGFX was created to meet the growing demands of my game engine, in which the regular high-level abstraction of Astral.Canvas could no longer meet.

Despite this, LGFX is *still* remarkably simple to use if you already know 'vanilla' Vulkan. A triangle takes less than 180 LOC! 

By itself, it does not come with any windowing library or input system. However, the higher-level helper API (lgfx-astral) uses GLFW and is meant to help users kickstart simple graphical apps by further bootstrapping trivial matters like instance and device creation and providing a render loop.

## Building
1. Install Premake5 ([If you don't have it already](https://premake.github.io/))
2. cd into repo directory, run premake5 (.vscode settings are configured for windows/vs2022)
3. Compile with the build system of your choice
4. To run examples, remember to copy the .shaderobj files found in the individual example project folders into the build output directory!