#version 330

uniform sampler2D u_texture;

uniform bool u_fog;
uniform vec3 u_sky_colour;
in float visibility;

out vec4 FragColor;
in vec2 uv;

void main()
{
    FragColor = texture(u_texture, uv);

    if(u_fog){
        FragColor = mix(vec4(u_sky_colour, 1.0), FragColor, visibility);
    }
}