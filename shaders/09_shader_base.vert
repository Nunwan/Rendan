#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 0) out vec2 fragTexCoord;

layout(binding = 0) uniform MeshConstants {
    mat4 matrix;
} camera;



void main() {
    gl_Position = camera.matrix * vec4(position, 1.0);
    fragTexCoord = uv;
}
