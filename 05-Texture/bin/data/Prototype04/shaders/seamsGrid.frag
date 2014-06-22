uniform sampler2DRect backbuffer;
uniform sampler2DRect edgeTexture;

uniform vec2 resolution;
uniform vec2 header;

uniform float fade;

void main(void){
	vec2 st = gl_TexCoord[0].st;//gl_FragCoord.xy;
	vec4 color = texture2DRect(edgeTexture,st);

	if( floor(st.y) == 0.0){
		gl_FragColor = color;
	} else if( floor(st.y) == header.y){
		vec2 offset = floor(st)+vec2(-0.5,-1.5);
		float minVal = 10.0;

		for(int i = 0; i < 3; i++){
			float v = texture2DRect(backbuffer,offset).r;
			if (v < minVal){
				minVal = v;
			}
			offset.x += 1.0;
		}
		color.r /= resolution.y*(0.01*fade);
		gl_FragColor = vec4(vec3(minVal+color.r),1.0);
	} else {
		gl_FragColor = texture2DRect(backbuffer,st);
	}	
}