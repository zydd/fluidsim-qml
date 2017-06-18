#version 130
#extension GL_ARB_shading_language_420pack: require

in highp vec2 uv;
out highp vec4 fragColor;

layout(binding = 0) uniform sampler2D dom;
layout(binding = 1) uniform sampler2D den;

void main() {
    const ivec2 UV = ivec2(gl_FragCoord.xy)/2;
    float obs = texture(den,uv).x;
    vec4 fc = texture(dom,uv);
    fragColor = obs*vec4(fc.w,fc.w*2.5f,fc.w,1) + (1-obs)*vec4(0.3f,0.2f,1.0f,1);
//    fragColor = vec4(vec3(sqrt(dot(fc.xy,fc.xy))/4),1);
//    fragColor = vec4(fc.xy+0.5,1,1);
//    fragColor = vec4(fc.zzz/2,1);
}
