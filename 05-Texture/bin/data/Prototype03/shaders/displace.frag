uniform sampler2DRect backbuffer;
uniform sampler2DRect normals;
uniform sampler2DRect dampMap;

void main(){
  vec2 st = gl_TexCoord[0].st;

  vec4 newFrame = texture2DRect(backbuffer, st);
  vec4 color = vec4(0,0,0,0);
  vec2 norm	= ( texture2DRect(normals, st).rg - 0.5 ) * 2.0;
  float damp = texture2DRect( dampMap, st ).r;
  float inc	= ( abs(norm.x) + abs(norm.y) ) * 0.5;
  
  vec2 offset[36];
  vec2 oposite[36];
  float fTotal = 36.0;
  int iTotal = 36;

  float pi = 3.14159265358979323846;
  float step = (pi*2.0)/fTotal;
  float angle = 0.0;
  for (int i = 0; i < iTotal; i++) {
    offset[i].x = cos(angle);
    offset[i].y = sin(angle);
    oposite[i].x = cos(angle+pi);
    oposite[i].y = sin(angle+pi);
    angle += step;
  }
  
  float sources = 0.0;
  for (int i = 0; i < iTotal; i++){
    vec4 goingTo = ( texture2DRect( normals, st + offset[i] ) - 0.5 ) * 2.0;
    if ( dot(goingTo.rg,offset[i]) < 1.0/fTotal ){
      sources += 1.0;
      color += texture2DRect(backbuffer, st + oposite[i]);
    }
  }
  
  color = color / sources;
  inc = 1.0-damp;
  
  gl_FragColor = mix(color,newFrame,inc);
}