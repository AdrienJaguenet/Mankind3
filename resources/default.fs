#version 130

uniform sampler2D texture0;
in vec3 fragment_position;
in vec2 fragment_uv;
out vec4 fragment_color;

void main(){
  fragment_color = texture(texture0, fragment_uv);
}
