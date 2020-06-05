#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 tex2dCoords;
uniform sampler2D ourTexture;
// TODO add a uniform float

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    vec2 tmp = vec2(0.805f * tex2dCoords.x+0.097, 0.68f * tex2dCoords.y+0.1);
    fragColor = texture(ourTexture,tmp);
}
