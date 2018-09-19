#version 440

in highp vec2 uv;
layout(location = 0) out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D den;

layout(location = 0) uniform int idx;
layout(location = 1) uniform vec2 size;
layout(location = 2) uniform vec2 point;
layout(location = 3) uniform float rad;
layout(location = 4) uniform float ratio;
layout(location = 5) uniform float density;

void main() {
    const ivec2 UV = ivec2(gl_FragCoord.xy);
//    vec2 coord = 2*uv-1;
//    float phy = mod(atan(coord.y,coord.x), 2.*3.14159265359);

    switch (idx) {
    case 0:
//        fragColor = vec4(0,0,1,texture(den,uv).x*7);
//        if (UV.x != 0 && UV.y != 0 && UV.x != size.x-1 && UV.y != size.y-1)
//            fragColor = vec4(0,0,1,0);
//        else
            fragColor = vec4(0,0,1,0);
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
        fragColor = texelFetch(den, UV, 0);
        fragColor.z += clamp(1 - 3 / rad*sqrt(dot(st, st)), 0, 1);
        fragColor.w += density * clamp(1 - 1 / rad * sqrt(dot(st, st)), 0, 1);
        break;
    }
    case 3: {
        fragColor = texelFetch(den, UV, 0);
        if (UV.y == 1 && UV.x > size.x/2 && UV.x < size.x-1)
            fragColor.zw = vec2(2,1);
        if (UV.y == size.y-2 && UV.x > 0 && UV.x < size.x/2)
            fragColor.zw = vec2(2,-2);
    }
    }
}
