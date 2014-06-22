uniform sampler2DRect backbuffer;
uniform sampler2DRect newFrame;
uniform float paperAbsortion;

void main(){
  vec4 color = texture2DRect(backbuffer, gl_TexCoord[0].st);

  float waterAmount = max(0.0,color.r-paperAbsortion*0.05);

  gl_FragColor = vec4(waterAmount,0.0,0.0,1.0);
}