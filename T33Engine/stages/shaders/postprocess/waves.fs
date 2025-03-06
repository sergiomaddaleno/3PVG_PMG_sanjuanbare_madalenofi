#version 330

uniform sampler2D u_screen_tex;
uniform float u_waves;

in vec2 uv;
out vec4 fragColor;

void main() {

    vec2 distorsion = uv + vec2(sin(uv.y * 10.0), cos(uv.x * 10.0)) * u_waves;

    fragColor = texture(u_screen_tex, distorsion);
}
