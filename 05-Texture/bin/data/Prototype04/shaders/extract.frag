uniform sampler2DRect extractTexture;
uniform sampler2DRect sourceTexture;
uniform vec2 resolution;
uniform float fade;

void main(){
	vec2 st = gl_TexCoord[0].st;

	vec4 color = vec4(0.0);
	float seamsX = texture2DRect(extractTexture, vec2(0.0,st.y)).r*resolution.x+0.5;

	if (seamsX == st.x+1.0){
		vec4 A = texture2DRect(sourceTexture, st+vec2(1.0,0.0));
		vec4 B = texture2DRect(sourceTexture, st-vec2(2.0,0.0));
		color = mix(A,B,fade);
	} else if (seamsX < st.x+1.0){
		color = texture2DRect(sourceTexture, st+vec2(1.0,0.0));
	} else {
		color = texture2DRect(sourceTexture, st);
	}

	gl_FragColor = color;
}