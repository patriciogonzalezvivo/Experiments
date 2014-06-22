uniform sampler2DRect backbuffer;
uniform sampler2DRect colorAddedTexture;

uniform float amount;

void main(){
	vec2 st = gl_TexCoord[0].st;
  	vec4 color = texture2DRect(backbuffer,st);
	vec4 newColor = texture2DRect(colorAddedTexture,st);
	float pct = newColor.a*amount;
  	gl_FragColor = vec4(mix(color.rgb,newColor.rgb,pct),1.0);
}