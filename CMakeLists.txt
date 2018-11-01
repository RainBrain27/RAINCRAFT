cmake_minimum_required(VERSION 3.0)

project(RAINCRAFT)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(OpenGL REQUIRED)



set(SRC
    main.cpp
    gl_main.cpp
    shape.cpp
    baseobject.cpp
    chunk.cpp
    chunk_obj.cpp
    terrain_generator.cpp

    common/shader.cpp
    common/texture.cpp
    common/controls.cpp
    common/objloader.cpp
    common/vboindexer.cpp
    )


add_executable(${PROJECT_NAME} ${SRC})

# Compile external dependencies
add_subdirectory (external)

include_directories(
        #external/AntTweakBar-1.16/include/
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glew-1.13.0/include/
        external/assimp-3.0.1270/include/
        #external/bullet-2.81-rev2613/src/
        common/
        shaders/
        external/SimplexNoise/
        .
)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
)


add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)
target_link_libraries(RAINCRAFT
       ${ALL_LIBS}
)
