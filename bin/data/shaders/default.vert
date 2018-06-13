#version 130

uniform mat4 mvp;

in vec3 vert;
in vec2 vertTexCoord;
out vec2 fragTexCoord;

out vec2 coord;

void main() {
    // Pass the tex coord straight through to the fragment shader
    fragTexCoord = vertTexCoord;
    gl_Position = vec4(vert, 1.0f);
}
