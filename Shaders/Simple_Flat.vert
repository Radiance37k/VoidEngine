#version 450

layout(location = 0) in vec3 inPosition;  // Vertex position

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    // Transform the vertex position into clip space
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    gl_Position = vec4(inPosition, 1.0);
}