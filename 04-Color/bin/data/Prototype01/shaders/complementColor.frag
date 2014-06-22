uniform sampler2DRect tex0;
uniform vec3 colorTarget;
uniform float threshold;

vec3 rgb2hsv(vec3 c){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void){

    vec3 colorTargetHUE = rgb2hsv(colorTarget);

    vec3 color = texture2DRect(tex0, gl_TexCoord[0].st).rgb;
    vec3 colorHUE = rgb2hsv(color);

    float dist = colorHUE.r-colorTargetHUE.r;

    if(dist < -0.5){
        dist = (colorHUE.r+1.0)-colorTargetHUE.r; 
    } else if (dist > 0.5){
        dist = colorHUE.r - (colorTargetHUE.r+1.0);
    }
    dist = abs(dist);

    if (dist < (threshold*0.5)){
        gl_FragColor = mix(vec4(color,1.0),vec4(0.0),dist);
    } else {
        gl_FragColor = vec4(0.0);
    }
}
