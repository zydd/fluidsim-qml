#version 440

in highp vec2 uv;
out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D dom;
layout(binding = 1) uniform sampler2D den;

layout(location = 0) uniform int display;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    const ivec2 UV = ivec2(gl_FragCoord.xy)/2;
    vec4 ic = texture(den,uv);
    vec4 fc = texture(dom,uv);
    float obs = ic.x;
    float d = fc.w * fc.w;
    switch (display) {
    case 0: fragColor = fc.w > 0 ? obs*vec4(d*0.4,d*1.3,d*2.5,1) + (1-obs)*vec4(0.3,0.2,1,1)
                                 : obs*vec4(d*2.5,d*1.3,d*0.4,1) + (1-obs)*vec4(1,0.2,0.3,1); break;
    case 1: fragColor = vec4(vec3(sqrt(dot(fc.xy,fc.xy))/3),1); break;
    case 2: fragColor = vec4(fc.xy+0.5,1,1); break;
    case 3: fragColor = vec4(fc.zzz/2,1); break;
    case 4: fragColor = vec4(clamp(ic.z, 0, 1), clamp(-ic.z, 0, 1), 0, 1); break;
    case 5: fragColor = vec4(log(1+0.5*log(1+abs(fc.www))),1); break;
    case 6: fragColor = vec4(hsv2rgb(vec3(clamp(-0.6+log(d)*0.4,0,1),1,d/2)),1); break;
    }
}
