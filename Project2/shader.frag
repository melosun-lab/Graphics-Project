#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;
in vec3 ExtraOutputColor;
in vec4 worldPosition;
in vec4 worldNormal;

uniform vec3 color;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform int normal_coloring;
uniform vec3 direct_light_color;
uniform vec3 direct_light_dir;
uniform vec3 point_light_color;
uniform vec3 point_light_pos;
uniform int dark;

//uniform vec3 lightColor;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

vec3 CalcDirLight(vec3 direction, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(vec3 direction, vec3 normal, vec3 viewDir);

vec3 CalcDirLight(vec3 direction, vec3 normal, vec3 viewDir)
{
    // diffuse shading
    float diff = max(dot(normal, direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 Ambient  = direct_light_color * ambient;
    vec3 Diffuse  = direct_light_color * (diff * diffuse);
    vec3 Specular = direct_light_color * (spec * specular);
    return (Ambient + Diffuse + Specular);
}

vec3 CalcPointLight(vec3 direction, vec3 normal, vec3 viewDir, float dist)
{
    // diffuse shading
    float diff = max(dot(normal, direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * (dist * dist));
    
    // combine results
    vec3 Ambient  = point_light_color * ambient;
    vec3 Diffuse  = point_light_color * (diff * diffuse);
    vec3 Specular = point_light_color * (spec * specular);
    
    Ambient *= attenuation;
    Diffuse *= attenuation;
    Specular *= attenuation;
    
    return (Ambient + Diffuse + Specular);
}

void main()
{
    vec3 fragPos = vec3(worldPosition.x,worldPosition.y,worldPosition.z);
    vec3 normal = normalize(vec3(worldNormal.x,worldNormal.y,worldNormal.z));
    
    vec3 lightDir = normalize(-vec3(direct_light_dir.x, direct_light_dir.x, direct_light_dir.z));
    
    vec3 viewDir = normalize(vec3(0, 0, 20) - fragPos);
    
    vec3 result_1 = CalcDirLight(lightDir, normal, viewDir);
    
    lightDir = normalize(point_light_pos - fragPos);
    
    float dist = length(point_light_pos - fragPos);
    
    vec3 result_2 = CalcPointLight(lightDir, normal, viewDir, dist);
    
    vec3 result = result_1 + result_2;
    
    result = result * color;
    
    if(dark  == 1){
        result = 0.1 * color;
    }
    
    vec3 myColor = normalize(ExtraOutputColor);
    
    if (normal_coloring == 1) {
        myColor.x = 0.5f * myColor.x + 0.5f;
        myColor.y = 0.5f * myColor.y + 0.5f;
        myColor.z = 0.5f * myColor.z + 0.5f;
        fragColor = vec4(color, sampleExtraOutput);
        fragColor = vec4(myColor, 1.0f);
    }
    else{
        fragColor = vec4(result, 1.0);
    }
    
}
