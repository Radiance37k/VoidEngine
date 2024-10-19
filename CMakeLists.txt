cmake_minimum_required(VERSION 3.10)

# Project name
project(VoidEngine3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#if (MSVC)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
#endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

# Manually specify the path to GLFW
set(GLFW_ROOT "/Forritun/Mitt/VoidEngine/Source/External/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY "${GLFW_ROOT}/lib-vc2022/glfw3.lib")

# Manually specify the path to GLM (it's header-only)
set(GLM_INCLUDE_DIR "Source/External/glm")

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
        Source/Components/Device.cpp
        Source/Components/Device.hpp
        Source/Components/Model.cpp
        Source/Components/Model.hpp
        Source/Components/RenderPipeline.cpp
        Source/Components/RenderPipeline.hpp
        Source/Components/SwapChain.cpp
        Source/Components/SwapChain.hpp
        Source/Components/Window.cpp
        Source/Components/Window.hpp
        Source/Core/GameObject.cpp
        Source/Core/GameObject.hpp
        Source/Managers/WindowManager.cpp
        Source/Managers/WindowManager.hpp
        Source/Managers/CameraManager.cpp
        Source/Managers/CameraManager.hpp
        Source/Managers/InputManager.cpp
        Source/Managers/InputManager.hpp
        Source/Managers/ModelManager.cpp
        Source/Managers/ModelManager.hpp
        Source/Managers/RenderManager.cpp
        Source/Managers/RenderManager.hpp
        Source/Managers/SceneManager.cpp
        Source/Managers/SceneManager.hpp
        Source/Managers/UIManager.cpp
        Source/Managers/UIManager.hpp
        Source/Managers/LightSourceManager.cpp
        Source/Managers/LightSourceManager.hpp
        Source/Components/Buffer.hpp
        Source/Components/Buffer.cpp
        Source/Components/Renderer.cpp
        Source/Components/Renderer.hpp
        Source/Components/Descriptors.cpp
        Source/Components/Descriptors.hpp
        Source/Components/PointLight.cpp
        Source/Components/PointLight.hpp
        Source/Components/FrameInfo.hpp
)

# Link Vulkan and GLFW to the DLL
target_link_libraries(VoidEngine PRIVATE
        Vulkan::Vulkan
        ${GLFW_LIBRARY}
)

# Define VOIDENGINE_EXPORTS when compiling the shared library (DLL)
target_compile_definitions(VoidEngine PRIVATE VOIDENGINE_EXPORTS)

# Create the executable (game)
add_executable(test1 Testbeds/Test1.cpp)

# Link the executable with the shared library (DLL)
target_link_libraries(test1 PRIVATE VoidEngine)

# Specify where the DLL should be copied to for execution
add_custom_command(TARGET test1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:VoidEngine>" "$<TARGET_FILE_DIR:test1>")

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