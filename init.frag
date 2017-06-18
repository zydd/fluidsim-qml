#version 130
#extension GL_ARB_shading_language_420pack: require
#extension GL_ARB_explicit_uniform_location: require
#extension GL_ARB_explicit_attrib_location: require

in highp vec2 uv;
layout(location = 0) out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D dom;
layout(binding = 1) uniform sampler2D den;

layout(location = 0) uniform int idx;

void main() {
    vec2 coord = 2*uv-1;
    float phy = mod(atan(coord.y,coord.x), 2.*3.1415926536);

    if (idx == 0) {
//        vec2 vel = vec2(-sin(phy),cos(phy))*3;
//        vec2 p = vec2(0,0.20);
//        vel = vec2(0,-10) * (dot(coord-p,coord-p) < 0.005 ? 1 : 0);
        vec2 vel = vec2(0,0);
        fragColor = vec4(vel,1,0);
    } else {
        fragColor = vec4(texture(den,uv).x*7,1,0,0);
        if (uv.t > 0.5 && uv.t < 5./6) {
            if (abs(fract(6*uv.t) - 0.5) < 0.3 && abs(fract(6*uv.s) - 0.5) < 0.3)
                fragColor.y = 0;
        }
    }
}
