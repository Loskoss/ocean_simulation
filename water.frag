#version 330 core

in vec3 viewDirection; // Input variable for view direction

out vec4 fragColor; // Output variable for fragment color

uniform float time; // Time variable for animation

void main()
{


    // Base color of the water
    vec3 baseColor = vec3(0.0, 0.4, 0.8);

    // Component of the water color influenced by the view direction
    // The water appears brighter when viewed from a direction closer to the surface normal (upward direction)
    vec3 viewDirectionColor = vec3(0.1, 0.1, 0.3) * max(0.0, dot(normalize(viewDirection), vec3(0.0, 1.0, 0.0)));

    // Component of the water color influenced by time
    // This adds a wave-like animation effect to the water color
    vec3 timeColor = vec3(0.0, 0.05, 0.1) * cos(time * 2.0 + gl_FragCoord.y / 100.0);

    // Combine the base color with the view direction color and time color
    vec3 waterColor = baseColor + viewDirectionColor + timeColor;

    // Output final color
    fragColor = vec4(waterColor, 1.0);
}
