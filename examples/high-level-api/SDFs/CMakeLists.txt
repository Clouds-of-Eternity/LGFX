add_executable(AstralCanvasSDFs)

target_sources(AstralCanvasSDFs PRIVATE
    "Program.cpp"
    "Json.cpp"
    "ModelLoader.cpp")

target_link_libraries(AstralCanvasSDFs PUBLIC
    LGFX
    Astral.Canvas.LGFX)

target_include_directories(AstralCanvasSDFs PUBLIC
    "../../../Astral.Core/Astral.Core")

add_custom_command(
    TARGET AstralCanvasSDFs  
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/DrawSDFs.shader"
        "${CMAKE_CURRENT_SOURCE_DIR}/CreateMeshSDF.shader"
        "${CMAKE_CURRENT_SOURCE_DIR}/Grass_Diffuse.png"
        "${CMAKE_CURRENT_SOURCE_DIR}/Grass_Displacement.png"
        $<TARGET_FILE_DIR:AstralCanvasSDFs>
)