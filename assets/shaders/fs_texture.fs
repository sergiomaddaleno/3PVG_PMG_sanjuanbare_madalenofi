
#version 330

out vec4 FragColor;
in vec2 uv;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, uv); 
}
