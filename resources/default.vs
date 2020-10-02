#version 130

in vec3 position;
in vec2 uv;
out vec3 fragment_position;
out vec2 fragment_uv;

void main() {
  gl_Position = vec4(position, 1.0);
  fragment_position = position;
  fragment_uv = uv;
}

