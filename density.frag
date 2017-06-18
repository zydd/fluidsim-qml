#version 130
#extension GL_ARB_shading_language_420pack: require
#extension GL_ARB_explicit_uniform_location: require
#extension GL_ARB_explicit_attrib_location: require

in highp vec2 uv;
out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D dom;
layout(binding = 1) uniform sampler2D den;

layout(location = 0) uniform float dt;
layout(location = 1) uniform vec2 inv_size;

vec2 random2(vec2 st){
  st = vec2( dot(st,vec2(127.1,311.7)),
      dot(st,vec2(269.5,183.3)) );
  return fract(sin(st)*43758.5453123);
}

void main() {
    vec2 coord = 2*uv-1;
    coord.y *= inv_size.y/inv_size.x;
    const ivec2 UV = ivec2(gl_FragCoord.xy);
    vec4 Density = texelFetch(den,UV,0);

    if (Density.y > 0.1) {
        vec2 vel = texture(dom,uv).xy;
        vec2 prev = uv - dt*vel*inv_size;
        Density.x = texture(den,prev).x;
//        vec2 p = vec2(-0.2,-0.9);
//        if (dot(coord-p,coord-p) < 0.001)
//            Density.x = 0.5;
//        if (random2(uv+vel+Density.x).x == 0)
//            Density.x += 0.03;
    }

    fragColor = Density;
}
