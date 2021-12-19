#version 420

uniform sampler2D tex;

in vec2 TexCoord; 

out vec4 FragColor;

void main(void) {
  FragColor = texture(tex, TexCoord); 
}
