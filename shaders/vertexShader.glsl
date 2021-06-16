#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexPos;

out vec3 color;
out vec2 texPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float offsetX;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    texPos = aTexPos;
}
