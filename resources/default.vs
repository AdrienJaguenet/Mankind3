#version 130

in vec3 position;
out vec3 fragment_position;

void main() {
  gl_Position = vec4(position, 1.0);
  fragment_position = position;
}
