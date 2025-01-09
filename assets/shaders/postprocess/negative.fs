#version 330

uniform sampler2D u_screen_tex;
uniform float u_negative;

in vec2 uv;
out vec4 fragColor;

void main() {
	
	vec4 color = texture(u_screen_tex, uv);

	fragColor = vec4(u_negative - color.rgb, color.a);
}