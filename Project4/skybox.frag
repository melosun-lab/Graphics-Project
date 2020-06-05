#version 330 core

in vec3 TexCoords;
//in vec3 color;

uniform samplerCube skybox;
//uniform float mode;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
        fragColor = texture(skybox, TexCoords);
}
