#version 440

in highp vec2 uv;
layout(location = 0) out highp vec4 dom_out;
layout(location = 1) out highp vec4 den_out;

layout(binding = 0) uniform sampler2D dom; // (v.x, v.y, pressure, density)
layout(binding = 1) uniform sampler2D den; // (obstacles, source, vorticity, _)

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
    case 0: default:
        dom_out = vec4(0,0,2,0);

        vec4 de = texelFetch(den, UV, 0);
        den_out.x = de.x; // obstacles
        den_out.y = (de.y - de.z) * 0.005; // source
        den_out.z = 0; // vorticity
        break;
    case 2: {
        vec2 st = uv;
        st.x *= ratio;
        st -= point * vec2(ratio,1);
        dom_out = texelFetch(dom, UV, 0);
        dom_out.z += clamp(1 - 3 / rad*sqrt(dot(st, st)), 0, 1);
        dom_out.w += density * clamp(1 - 1 / rad * sqrt(dot(st, st)), 0, 1);

        den_out = texelFetch(den, UV, 0);
        break;
    }
    }
}
