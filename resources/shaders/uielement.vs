#version 420

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

uniform int screen_width_px;
uniform int screen_height_px;
uniform int width_px;
uniform int height_px;
uniform int top_px;
uniform int left_px;

out vec2 TexCoord;

void main() {
  float top = float(top_px) / float(screen_height_px);
  float left = float(left_px) / float(screen_width_px);
  float width = float(width_px) / float(screen_width_px);
  float height = float(height_px) / float(screen_height_px);
  gl_Position = vec4(left + position.x * width - .5, top + position.y * height - .5, 0, 1.0);
  TexCoord = tex_coord;
}
