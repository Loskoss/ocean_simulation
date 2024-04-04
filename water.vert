#version 330 core

layout(location = 0) in vec3 vertexPosition; // Vertex position attribute

uniform mat4 modelMatrix; // Model matrix
uniform mat4 viewMatrix; // View matrix

out vec3 viewDirection; // Output variable for view direction

void main()
{
    vec4 modelPosition = modelMatrix * vec4(vertexPosition, 1.0); // Transform vertex position to model space
    vec4 viewPosition = viewMatrix * modelPosition; // Transform model position to view space
    viewDirection = -normalize(viewPosition.xyz); // Calculate view direction
    gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0); // Transform vertex position to clip space
}