#version 440

in highp vec2 uv;
layout(location = 0) out highp vec4 dom_out;
layout(location = 1) out highp vec4 den_out;

layout(binding = 0) uniform sampler2D dom; // (v.x, v.y, pressure, density)
layout(binding = 1) uniform sampler2D den; // (obstacles, source, vorticity, _)

layout(location = 0) uniform float dt;
layout(location = 1) uniform vec2 inv_size;
layout(location = 2) uniform float v;
layout(location = 3) uniform float K;
layout(location = 4) uniform vec2 g;

vec2 random2(vec2 st){
  st = vec2(dot(st, vec2(127.1, 311.7)),
            dot(st, vec2(269.5, 183.3)));
  return fract(sin(st) * 43758.5453123);
}

bool is_boundary(vec2 uv) {
  return uv.s <= inv_size.s || uv.s > (1-inv_size.s) ||
      uv.t <= inv_size.t || uv.t > (1-inv_size.t);
}

void main() {
    vec2 coord = 2*uv-1;
//    float phy = mod(atan(coord.y, coord.x), 2.0 * 3.14159265359);

    const ivec2 UV = ivec2(gl_FragCoord.xy);
    vec4 fc = texelFetch(dom, UV, 0);
    vec4 ic = texelFetch(den, UV, 0);

    if (ic.x > 0) {
        const vec3 fr = texelFetch(dom, UV + ivec2(1, 0), 0).xyz;
        const vec3 fl = texelFetch(dom, UV - ivec2(1, 0), 0).xyz;
        const vec3 ft = texelFetch(dom, UV + ivec2(0, 1), 0).xyz;
        const vec3 fb = texelFetch(dom, UV - ivec2(0, 1), 0).xyz;

        fc.xyw = texture(dom, uv - dt * fc.xy * inv_size).xyw;
        fc.w += ic.y;
        vec2 acc = fc.w * g;

        // grad p = (d/dx p, d/dy p)
        vec2 grad_p = vec2((fr.z - fl.z) / 2, (ft.z - fb.z) / 2);

        // div ú = d/dx u_x + d/dy u_y
        float div_u = (fr.x - fl.x)/2 + (ft.y - fb.y) / 2;

        // lap ú = (d2/dx2 u_x + d2/dy2 u_x, d2/dx2 u_y + d2/dy2 u_y)
        vec2 lap_u = (fr.xy - 2 * fc.xy + fl.xy) / 4;

        // dp/dt + div pú = 0
        // dp/dt = -ú . grad p - p div ú
        // dp = (-ú . grad p - p div ú) dt
        // p = p_ + (-ú . grad p - p div ú) dt  // dp = p - p_
        float p = fc.z + (-dot(fc.xy, grad_p) - fc.z * div_u) * dt;
        p += (2 - p) * 0.001;
        fc.z = clamp(p, 0.2, 100);

        // grad P ≃ K grad p
        // du/dt = - grad P / p + ǵ + mu/p lap u
        // ú = ú_ + ( - K/p grad p + g + mu/p lap ú) dt  // du = ú - ú_
        fc.xy += (-K/fc.z * grad_p + acc + v/fc.z * lap_u) * dt;

        if (texelFetch(den, UV + ivec2( 1, 0), 0).x < 0.1
                || texelFetch(den, UV + ivec2(-1, 0), 0).x < 0.1) {
//            fc.y += abs(fc.x) * sign(fc.y);
            fc.x = 0;
        } if (texelFetch(den, UV + ivec2( 0,-1), 0).x < 0.1
                || texelFetch(den, UV + ivec2( 0, 1), 0).x < 0.1) {
//            fc.x += abs(fc.y) * sign(fc.x);
            fc.y = 0;
        }

//        vec2 rnd = random2(uv+fc.xy); fc.w += rnd.x == 0 && rnd.y == 0 ? 1 : 0;
    }

    dom_out = fc;
    den_out = ic;
}
