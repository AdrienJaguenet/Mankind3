#version 130

in vec3 position;
in vec3 normal;
in vec2 uv;
out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_uv;

uniform mat4 model, view, projection;

void main() {
  gl_Position = (projection * view * model) * vec4(position, 1.0);
  fragment_position = position;
  fragment_normal = normal;
  fragment_uv = uv;
}

