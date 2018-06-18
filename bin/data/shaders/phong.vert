#version 130

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 eye;

in vec3 vertInPos;
in vec3 vertInNormal;
in vec2 vertInUV;

out vec2 outUV;
out vec3 outNormal;
out vec3 outViewVec;
out vec3 outLightVec;

void main()
{
   	outUV           = vertInUV;
	gl_Position     = projection * view * model * vec4(vertInPos.xyz, 1.0);
    vec4 pos        = vec4(vertInPos, 1.0);
	outNormal       = mat3((model)) * vertInNormal;
	vec3 lPos       = eye.xyz;
	lPos.x          = lPos.x + 3.f;
    outLightVec     = lPos    - pos.xyz;
    outViewVec      = eye.xyz - pos.xyz;
}