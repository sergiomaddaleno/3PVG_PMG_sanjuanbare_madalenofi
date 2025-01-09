#version 330

uniform sampler2D screen_tex;
float blur = 0.01;

in vec2 uv;
out vec4 fragColor;

void main() {
    
    vec4 sum = vec4(0.0);

    float blurWeights[9] = float[](0.05, 0.09, 0.12, 0.15, 0.18, 0.15, 0.12, 0.09, 0.05);
    vec2 blurDirections[9] = vec2[](
        vec2(-1.0, -1.0), vec2( 0.0, -1.0), vec2( 1.0, -1.0),
        vec2(-1.0,  0.0), vec2( 0.0,  0.0), vec2( 1.0,  0.0),
        vec2(-1.0,  1.0), vec2( 0.0,  1.0), vec2( 1.0,  1.0)
    );

    for (int i = 0; i < 9; ++i) {
        sum += texture(screen_tex, uv + blur * blurDirections[i]) * blurWeights[i];
    }
    
    //fragColor = sum;
    fragColor = texture(screen_tex, uv);
}
