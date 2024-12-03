#version 330

uniform sampler2D u_texture;
in vec2 uv;
out vec4 fragColor;

void main() {
    // Tamaño del desenfoque
    float blurSize = 0.01;
    
    // Acumulador de color
    vec4 sum = vec4(0.0);
    
    // Máscara de desenfoque gaussiano
    float blurWeights[9] = float[](0.05, 0.09, 0.12, 0.15, 0.18, 0.15, 0.12, 0.09, 0.05);
    vec2 blurDirections[9] = vec2[](
        vec2(-1.0, -1.0), vec2( 0.0, -1.0), vec2( 1.0, -1.0),
        vec2(-1.0,  0.0), vec2( 0.0,  0.0), vec2( 1.0,  0.0),
        vec2(-1.0,  1.0), vec2( 0.0,  1.0), vec2( 1.0,  1.0)
    );

    // Muestrear y aplicar desenfoque
    for (int i = 0; i < 9; ++i) {
        sum += texture(u_texture, uv + blurSize * blurDirections[i]) * blurWeights[i];
    }
    
    // Salida
    fragColor = sum;
}
