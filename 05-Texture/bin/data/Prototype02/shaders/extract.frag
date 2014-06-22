uniform sampler2DRect texture;
uniform vec2 resolution;
uniform vec2 offset;
uniform float time;

uniform float amount;
uniform float head_speed;
// uniform float fade1_speed;
// uniform float fade2_speed;

void main(){
	vec2 st = gl_TexCoord[0].st;
	vec2 off;
	off.x = (offset.x*0.5+abs(sin(time*head_speed))*0.5)*resolution.x;
	off.y = st.y+((offset.y*2.0)-1.0)*resolution.y;

	vec4 glitch = texture2DRect(texture,off)*amount;
	glitch -= amount*0.5;
	// glitch *= 0.5;

	// float fade = (sin(time*fade1_speed)-1.0)*0.5;
	// fade += (cos(time*fade2_speed)-1.0)*0.5;
	// fade *= 0.5;

	gl_FragColor = glitch; //mix(vec4(0.0),glitch,1.0);
	gl_FragColor.a = 1.0;
}