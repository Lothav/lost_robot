#version 330                                                                        

in vec3 vert;
in vec2 vertTexCoord;
out vec2 fragTexCoord;

in ivec4 boneIDs;
in vec4 weights;

const int MAX_BONES = 100;

uniform mat4 mvp;
uniform mat4 gBones[MAX_BONES];

void main()
{       
    fragTexCoord = vertTexCoord;

    mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
    BoneTransform     += gBones[boneIDs[1]] * weights[1];
    BoneTransform     += gBones[boneIDs[2]] * weights[2];
    BoneTransform     += gBones[boneIDs[3]] * weights[3];

    vec4 PosL    = BoneTransform * vec4(vert, 1.0);
    gl_Position  = mvp * PosL;
}
