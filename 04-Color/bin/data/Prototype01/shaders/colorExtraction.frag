uniform sampler2DRect tex0;
uniform vec3 colorTarget;
uniform float threshold;

void main(void){

    mat3 yuv2rgb = mat3(1.0, 1.0, 1.0,
                        0.0,-0.39465,2.03211,
                        1.13983,-0.58060,0.0);

    vec3 colorTargetYUV = yuv2rgb * colorTarget;

    vec3 color = texture2DRect(tex0, gl_TexCoord[0].st).rgb;
    vec3 colorYUV = yuv2rgb * color;

    float dist = distance(colorYUV.gb,colorTargetYUV.gb);
    if (dist < (threshold*0.5)){
        gl_FragColor = mix(vec4(color,1.0),vec4(0.0),dist);
    } else {
        gl_FragColor = vec4(0.0);
    }
}
