#version 130

precision mediump float;

uniform sampler2D tex;

in vec2 fragTexCoord;
out vec4 finalColor;

void main() {
    finalColor = vec4(1.f,1.f,1.f,1.f);
}
