#version 140

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
	outNormal       = mat3(inverse(transpose(model))) * vertInNormal;
	vec3 lightPos   = vec3(3.2f, 1.2f, 1.5f);
	vec3 lPos       = mat3(inverse(transpose(model))) * lightPos.xyz;
    outLightVec     = lPos    - pos.xyz;
    outViewVec      = eye.xyz - pos.xyz;
}