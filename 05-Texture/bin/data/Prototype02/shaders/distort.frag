


uniform sampler2DRect offsetTexture;
uniform sampler2DRect sourceTexture;
uniform vec2 resolution;

float map(float _value, float _minIn, float _maxIn, float _minOut, float _maxOut){
	return ((_value - _minIn) / (_maxIn - _minIn) * (_maxOut - _minOut) + _minOut);
}

float getColorChannel(vec2 _pos, int _channel){
	if( _pos.x>resolution.x || _pos.x<0.0 ){
		return 0.0;
	} else {
		vec4 color = texture2DRect(sourceTexture, _pos);
		return color[_channel] * color.a;
	}
}

void main(){
	vec2 st = gl_TexCoord[0].st;

	vec3 offset = texture2DRect(offsetTexture, vec2(0.5,st.y)).rgb;
	offset *= resolution.x;
	offset -= resolution.x*0.5;
	offset += st.x;

	float red = getColorChannel(vec2(offset.x,st.y),0);
	float green = getColorChannel(vec2(offset.y,st.y),1);
	float blue = getColorChannel(vec2(offset.z,st.y),2);

	vec4 color = vec4(red,green,blue, 1.0) ;

	gl_FragColor = color;
}