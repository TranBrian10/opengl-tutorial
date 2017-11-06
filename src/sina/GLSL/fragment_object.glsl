//FRAGMENT SHADER
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //For White boxes. No Textures.
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // For adding a single texture.
    //FragColor = texture(texture1, TexCoord);// * vec4(ourColor, 1.0); 
    // For adding multiple textures
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2f);
} 