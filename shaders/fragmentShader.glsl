#version 330 core

in vec2 texPos;

out vec4 FragColor;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float mixValue;

void main() {
    FragColor = mix(texture(tex1, texPos), texture(tex2, texPos), mixValue);
}
