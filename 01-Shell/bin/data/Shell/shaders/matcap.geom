#version 120

#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform float normalScale;
uniform float _useUV;

varying in vec3 vPos[3];
varying in vec2 vTexCoord[3];
varying in vec2 vUv[3];
varying in vec3 vNormal[3];
varying in vec3 vEye[3];
varying in vec3 vU[3];
varying in vec2 vN[3];

varying out vec2 Uv;
varying out vec3 Tangent;
varying out vec3 Binormal;
varying out vec3 Normal;
varying out vec3 Eye;
varying out vec3 U;
varying out vec2 N;

varying out vec3 Pos;

void main(void){

    vec3 sdir = vec3(1.0);
    vec3 tdir = vec3(1.0);
    Tangent = vec3(0.);
    Binormal = vec3(0.);

    if( normalScale > 0. && _useUV == 1.) {
        vec3 v1 = vPos[0].xyz;
        vec3 v2 = vPos[1].xyz;
        vec3 v3 = vPos[2].xyz;

        vec2 w1 = vTexCoord[0];
        vec2 w2 = vTexCoord[1];
        vec2 w3 = vTexCoord[2];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        sdir = vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        tdir = vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
    }

    for(int i = 0; i < 3; i++){
        Uv = vUv[i];

        Normal = vNormal[i];
        if( normalScale > 0. && _useUV == 1.) {
            vec3 t = normalize(sdir-vNormal[i]*dot(vNormal[i],sdir));
            float w = (dot(cross(vNormal[i],sdir),tdir)<0.0)? -1.0:1.0;

            Tangent = normalize( gl_NormalMatrix * t);
            Binormal = normalize( cross( Normal, Tangent ) * w );
        }

        Eye = vEye[i];

        U = vU[i];
        N = vN[i];
        gl_TexCoord[0].xy = vTexCoord[i];
        gl_Position = gl_PositionIn[i];
        Pos = vPos[i];
        EmitVertex();
    }

    EndPrimitive();
}