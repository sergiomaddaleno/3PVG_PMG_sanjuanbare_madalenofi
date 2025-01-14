#version 330

uniform sampler2D texture1;
uniform bool u_fog;
vec3 skyColour = vec3(0.5, 0.5, 0.5);

out vec4 FragColor;
in vec2 uv;
in float visibility;

void main()
{
    FragColor = texture(texture1, uv);

    if(u_fog){
        FragColor = mix(vec4(skyColour, 1.0), FragColor, visibility);
    }
}