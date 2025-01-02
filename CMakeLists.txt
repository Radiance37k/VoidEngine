cmake_minimum_required(VERSION 3.28)

# Project name
project(VoidEngine3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Manually specify the path to GLFW
set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/Source/External/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY "${GLFW_ROOT}/lib-vc2022/glfw3.lib")

# Manually specify the path to GLM (it's header-only)
set(GLM_INCLUDE_DIR "Source/External/glm")

# Set DEBUG definitions
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(
        Source
        Source/Core
        Source/Managers
        Source/Components
        ${Vulkan_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        External/tinyobjloader
)

# Create the shared library (DLL)
add_library(VoidEngine SHARED
        Source/Common.hpp
        Source/VoidEngine.cpp
        Source/VoidEngine.hpp

        Source/Components/Camera.cpp
        Source/Components/Camera.hpp
        Source/Components/GameObject.cpp
        Source/Components/GameObject.hpp
        Source/Components/Model.cpp
        Source/Components/Model.hpp
        Source/Components/PointLight.cpp
        Source/Components/PointLight.hpp

        Source/Core/Buffer.hpp
        Source/Core/Buffer.cpp
        Source/Core/Descriptors.cpp
        Source/Core/Descriptors.hpp
        Source/Core/Device.cpp
        Source/Core/Device.hpp
        Source/Core/FrameInfo.hpp
        Source/Core/Renderer.cpp
        Source/Core/Renderer.hpp
        Source/Core/RenderPipeline.cpp
        Source/Core/RenderPipeline.hpp
        Source/Core/SwapChain.cpp
        Source/Core/SwapChain.hpp
        Source/Core/Window.cpp
        Source/Core/Window.hpp

        Source/Managers/CameraManager.cpp
        Source/Managers/CameraManager.hpp
        Source/Managers/InputManager.cpp
        Source/Managers/InputManager.hpp
        Source/Managers/LightSourceManager.cpp
        Source/Managers/LightSourceManager.hpp
        Source/Managers/ModelManager.cpp
        Source/Managers/ModelManager.hpp
        Source/Managers/RenderManager.cpp
        Source/Managers/RenderManager.hpp
        Source/Managers/SceneManager.cpp
        Source/Managers/SceneManager.hpp
        Source/Managers/UIManager.cpp
        Source/Managers/UIManager.hpp
        Source/Managers/WindowManager.cpp
        Source/Managers/WindowManager.hpp
        Source/Components/Transform.cpp
        Source/Components/Transform.hpp
)

# Link Vulkan and GLFW to the DLL
target_link_libraries(VoidEngine PRIVATE
        Vulkan::Vulkan
        ${GLFW_LIBRARY}
)

# Define VOIDENGINE_EXPORTS when compiling the shared library (DLL)
target_compile_definitions(VoidEngine PRIVATE VOIDENGINE_EXPORTS)
target_compile_definitions(VoidEngine PRIVATE _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING)

# Create the executable tests (game)
add_executable(Test1 Testbeds/Test1.cpp)
add_executable(Test2 Testbeds/Test2.cpp)

# Link the executable with the shared library (DLL)
target_link_libraries(Test1 PRIVATE VoidEngine)
target_link_libraries(Test2 PRIVATE VoidEngine)

# Shader compilation
# Set directories for source and compiled shaders
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

# Make sure the shader binary directory exists
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

# Get all .vert and .frag files in the shader directory
file(GLOB_RECURSE SHADER_SOURCES
        "${SHADER_SOURCE_DIR}/*.vert"
        "${SHADER_SOURCE_DIR}/*.frag"
)

# Iterate over each shader and compile it to SPIR-V
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILENAME_WE ${SHADER} NAME)  # Get the filename without extension
    set(COMPILED_SHADER ${SHADER_BINARY_DIR}/${FILENAME_WE}.spv)

    if(NOT TARGET ${FILENAME_WE}_shader)
        add_custom_command(
                OUTPUT ${COMPILED_SHADER}
                COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${SHADER} -o ${COMPILED_SHADER}
                DEPENDS ${SHADER}
                COMMENT "Compiling ${SHADER} to SPIR-V"
        )

        add_custom_target(${FILENAME_WE}_shader ALL DEPENDS ${COMPILED_SHADER})
        list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
    endif()
endforeach()

# Create a common dependency for all shaders
add_custom_target(Shaders ALL DEPENDS ${COMPILED_SHADERS})

# Link shaders to your main target
add_dependencies(VoidEngine Shaders)