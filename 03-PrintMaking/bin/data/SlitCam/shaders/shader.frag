uniform sampler2DRect backBuffer;
uniform sampler2DRect tex0;

uniform float srcHeader;
uniform vec2  dstHeader;
uniform float grainSize;
uniform float time;
                                  
float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233)))*43758.5453);
}

float noise2f( in vec2 p ){
  vec2 ip = vec2(floor(p));
  vec2 u = fract(p);
  u = u*u*(3.0-2.0*u);

  float res = mix(mix(rand(ip),
                      rand(ip+vec2(1.0,0.0)),
                      u.x),
                  mix(rand(ip+vec2(0.0,1.0)),
                      rand(ip+vec2(1.0,1.0)),
                      u.x),
                  u.y);
  return res*res;
}

void main(){
  vec2 st = gl_TexCoord[0].st;
  vec4 color = texture2DRect(backBuffer, st);

  if ( abs(st.x - dstHeader) < srcHeader.y ){
    vec2 srcSt = vec2(srcHeader.x,st.y);
    float noise = noise2f( vec2(st.x*time*0.3,st.y) * grainSize );
    srcSt.x += srcHeader.y * noise*2.0;
    vec4 srcColor = texture2DRect(tex0, srcSt);
    color = mix(color,srcColor, noise );
  }

  gl_FragColor = color;
}