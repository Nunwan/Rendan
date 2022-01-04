#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform MeshConstants {
    mat4 matrix;
} camera;



void main() {
    gl_Position = camera.matrix * vec4(position, 1.0);
    fragColor = normal;
}
