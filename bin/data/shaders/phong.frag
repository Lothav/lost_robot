#version 130

uniform sampler2D inTexture;


in vec2 outUV;
in vec3 outNormal;
in vec3 outViewVec;
in vec3 outLightVec;

out vec4 outFragColor;

void main()
{
	vec4 color = texture(inTexture, outUV, 0.0);

    if(color.a < 0.5) {
        discard;
    }

	vec3 N = normalize(outNormal);
	vec3 L = normalize(outLightVec);
	vec3 V = normalize(outViewVec);
	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), .5) * vec3(.8);
	float specular = pow(max(dot(R, V), 0.0), 160.0) * color.a;

	outFragColor = vec4(diffuse  * color.rgb + specular, 1.0);
}