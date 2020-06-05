#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
in vec3 ExtraOutputColor;

//uniform vec3 lightColor;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

//in vec3 textureDir;

//uniform Cube cubemap;


void main()
{
    vec3 myColor = normalize(ExtraOutputColor);
    
    myColor.x = 0.5f * myColor.x + 0.5f;
    myColor.y = 0.5f * myColor.y + 0.5f;
    myColor.z = 0.5f * myColor.z + 0.5f;
    fragColor = vec4(myColor, 1.0f);
    //fragColor = texture(cubemap, textureDir);
}
