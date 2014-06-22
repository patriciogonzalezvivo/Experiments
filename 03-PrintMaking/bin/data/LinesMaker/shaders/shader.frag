uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform vec2 resolution;
uniform vec2 textureResolution;

uniform float grain;

void main(){
	vec2 st = gl_FragCoord.xy/resolution.xy;
	st.y = 1.0 - st.y;
	
	vec3 color = texture2DRect( tex0, st*resolution.xy ).rgb;
	
	if (grain>0.0){
		vec3 disp = texture2DRect(tex1, st*textureResolution.xy ).rgb;
		st.x += disp.x*grain;
		st.x -= disp.y*grain;
		color *= 0.1;
		color += texture2DRect( tex0, st*resolution.xy ).rgb*0.9;
	}
	
	gl_FragColor = vec4(color,1.0);

}