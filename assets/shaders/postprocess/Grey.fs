#version 330

uniform sampler2D u_texture;
in vec2 uv;
out vec4 fragColor;

void main() {
    // Obtener el color original de la textura
    vec4 originalColor = texture(u_texture, uv);
    
    // Convertir el color original a escala de grises
    float gray = dot(originalColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 grayscaleColor = vec3(gray);
    
    // Mezclar el color original con el color en escala de grises
    vec3 finalColor = mix(originalColor.rgb, grayscaleColor, 1.0);
    
    // Salida
    fragColor = vec4(finalColor, originalColor.a);
}
