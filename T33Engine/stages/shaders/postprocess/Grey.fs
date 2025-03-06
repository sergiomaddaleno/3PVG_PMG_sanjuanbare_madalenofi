#version 330

uniform sampler2D u_screen_tex;
uniform float u_grey;

in vec2 uv;
out vec4 fragColor;

void main() {

    vec4 originalColor = texture(u_screen_tex, uv);
    
    float grey = dot(originalColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 grayscaleColor = vec3(grey);
    
    vec3 finalColor = mix(originalColor.rgb, grayscaleColor, u_grey);
    
    fragColor = vec4(finalColor, originalColor.a);
}
