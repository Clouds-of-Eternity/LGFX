add_executable(AstralCanvasCompute)

target_sources(AstralCanvasCompute PRIVATE
    "Program.cpp"
    "Json.cpp")

target_link_libraries(AstralCanvasCompute PUBLIC
    LGFX
    Astral.Canvas.LGFX)

target_include_directories(AstralCanvasCompute PUBLIC
    "../../../Astral.Core/Astral.Core")

add_custom_command(
    TARGET AstralCanvasCompute  
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/DrawParticles.shaderobj"
        "${CMAKE_CURRENT_SOURCE_DIR}/UpdateParticles.shaderobj"
        $<TARGET_FILE_DIR:AstralCanvasCompute>
)