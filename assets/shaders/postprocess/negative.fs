#version 330

uniform sampler2D u_texture;
in vec2 uv;
out vec4 fragColor;

void main() {
	vec2 texCoord = uv;
	// Aquí comienza el efecto de postprocesamiento

	// Ejemplo de inversión de colores (negativo)
	vec4 color = texture(u_texture, texCoord);
	fragColor = vec4(1.0 - color.rgb, color.a);

	// Fin del efecto de postprocesamiento
}