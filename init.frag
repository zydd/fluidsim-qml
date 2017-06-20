#version 130
#extension GL_ARB_shading_language_420pack: require
#extension GL_ARB_explicit_uniform_location: require
#extension GL_ARB_explicit_attrib_location: require

in highp vec2 uv;
layout(location = 0) out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D den;

layout(location = 0) uniform int idx;
layout(location = 1) uniform vec2 size;
layout(location = 2) uniform vec2 point;
layout(location = 3) uniform float rad;
layout(location = 4) uniform float ratio;

void main() {
    const ivec2 UV = ivec2(gl_FragCoord.xy);
//    vec2 coord = 2*uv-1;
//    float phy = mod(atan(coord.y,coord.x), 2.*3.1415926536);

    switch (idx) {
    case 0:
//        fragColor = vec4(0,0,1,texture(den,uv).x*7);
        fragColor = vec4(0,0,0,0);
        break;
    case 1:
        fragColor = vec4(1);
//        if (uv.t > 0.5 && uv.t < 5./6) {
//            if (abs(fract(6*uv.t) - 0.5) < 0.3 && abs(fract(6*uv.s) - 0.5) < 0.3)
//                fragColor.x = 0;
//        }
        break;
    case 2: {
        vec2 st = uv;
        st.x *= ratio;
        st -= point * vec2(ratio,1);
        fragColor = texelFetch(den,UV,0) + 0.5*clamp(1-2/rad*sqrt(dot(st,st)),0,1);
        break;
    }
    }
}
