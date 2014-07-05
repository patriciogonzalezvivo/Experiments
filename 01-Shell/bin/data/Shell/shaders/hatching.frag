#version 120

uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D hatch3;
uniform sampler2D hatch4;
uniform sampler2D hatch5;
uniform sampler2D hatch6;

uniform float repeat;

uniform float useRim;
uniform float claroOscuro;

uniform float dephtScale;
uniform float dephtExp;

uniform float border;

uniform vec3 _inkColor;

varying vec2 vUv;
varying vec3 vNormal;
varying vec3 vPosition;
varying vec3 pos;

varying vec4 vPos;

vec3 getTriPlanarBlend(vec3 _wNorm){
    // in wNorm is the world-space normal of the fragment
    vec3 blending = abs( _wNorm );
    blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    return blending;
}

vec4 getTexture(sampler2D _tex, vec3 _normal, vec4 _pos, float _repeat){
    vec3 blending = getTriPlanarBlend(_normal);
    vec4 xaxis = texture2D( _tex, _pos.yz * _repeat);
    vec4 yaxis = texture2D( _tex, _pos.xz * _repeat);
    vec4 zaxis = texture2D( _tex, _pos.xy * _repeat);
    return xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;
}

vec4 getTexture(sampler2D _tex){
    return texture2D(_tex,vUv);
    // return getTexture(_tex,vNormal,vPos,repeat);
}

vec4 hatch(float shading){
    vec4 c = vec4(0.);
    float step = 1. / 6.;
    if( shading <= step ){
        c = mix( getTexture( hatch6 ), getTexture( hatch5 ), 6. * shading );
    } else if( shading > step && shading <= 2. * step ){
        c = mix( getTexture( hatch5 ), getTexture( hatch4 ) , 6. * ( shading - step ) );
    } else if( shading > 2. * step && shading <= 3. * step ){
        c = mix( getTexture( hatch4 ), getTexture( hatch3 ), 6. * ( shading - 2. * step ) );
    } else if( shading > 3. * step && shading <= 4. * step ){
        c = mix( getTexture( hatch3 ), getTexture( hatch2 ), 6. * ( shading - 3. * step ) );
    } else if( shading > 4. * step && shading <= 5. * step ){
        c = mix( getTexture( hatch2 ), getTexture( hatch1 ), 6. * ( shading - 4. * step ) );
    } else if( shading > 5. * step ){
        vec4 lightHatch = getTexture( hatch1 );
        c = mix( lightHatch, vec4( vec3(1.), min(claroOscuro,lightHatch.a) ), 6. * ( shading - 5. * step ) );
    }
    return c;
}

int lightsNumber = 8;
void DirectionalLight(in int i, in vec3 normal,inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP,nDotHV,pf;

    nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));
    nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient  += gl_LightSource[i].ambient;
    diffuse  += gl_LightSource[i].diffuse * nDotVP;
    specular += gl_LightSource[i].specular * pf;
}
void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP,nDotHV,pf,attenuation,d;
    vec3 VP,halfVector;

    VP = vec3(gl_LightSource[i].position) - ecPosition3;

    d = length(VP);

    VP = normalize(VP);

    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                         gl_LightSource[i].linearAttenuation * d +
                         gl_LightSource[i].quadraticAttenuation * d * d);

    halfVector = normalize(VP + eye);

    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient += gl_LightSource[i].ambient * attenuation;
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}

void SpotLight(in int i, in vec3 eye, vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP,nDotHV,pf,spotDot,spotAttenuation,attenuation,d;
    vec3 VP, halfVector;

    VP = vec3(gl_LightSource[i].position) - ecPosition3;

    d = length(VP);

    VP = normalize(VP);

    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                         gl_LightSource[i].linearAttenuation * d +
                         gl_LightSource[i].quadraticAttenuation * d * d);

    spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));

    if (spotDot < gl_LightSource[i].spotCosCutoff)
        spotAttenuation = 0.0;
    else
        spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);

    attenuation *= spotAttenuation;

    halfVector = normalize(VP + eye);

    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient  += gl_LightSource[i].ambient * attenuation;
    diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}

vec4 calc_lighting_shade(in vec3 _ecPosition, in vec3 _normal) {
    vec3 eye = vec3(0.0, 0.0, 1.0);

    vec4 amb  = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);

    for (int i = 0; i < lightsNumber; i++){
        if (gl_LightSource[i].position.w == 0.0)
            DirectionalLight(i, normalize(_normal), amb, diff, spec);
        else if (gl_LightSource[i].spotCutoff == 180.0)
            PointLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);
        else
            SpotLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);
    }

    float rim = max( 0., abs( dot( normalize( vNormal ), normalize( vPosition ) ) ) );

    vec4 color = gl_FrontLightModelProduct.sceneColor +
                amb * gl_FrontMaterial.ambient +
                diff * gl_FrontMaterial.diffuse +
                spec * gl_FrontMaterial.specular +
                ( 1. - smoothstep( 0.0, 1., rim ) ) * useRim;

    return hatch(length(color.rgb));
}

void main() {

    float depth = clamp(pow( length( pos ) / (10.0+dephtScale*1000.), dephtExp*10. ),0., 1. );
    vec4 color = vec4(0.);

    // if(!gl_FrontFacing)
    {
        color = calc_lighting_shade(vPosition,vNormal);

        if(dephtScale>0. && dephtExp>0.){
            color.a *= 1.0-depth;
        }
    }

    float cosTheta = abs( dot( normalize(vPosition), vNormal) );
    float fresnel = pow(1.0 - cosTheta, 30.);
    if(border>0.){
        if(fresnel>depth*(1.0-border )){
            color = vec4(0.,0.,0.,1.);
        }
    }

    // if(gl_FrontFacing)
        // gl_FragColor = vec4(0.);
    // else
        gl_FragColor = color;
        // gl_FragColor = vec4(vec3(1.0-color.a),1.0);
}