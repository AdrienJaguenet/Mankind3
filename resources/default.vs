#version 420

layout(location = 0) in int pack;

uniform mat4 model, view, projection;
uniform int tilemap_grid_size;

out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_uv;

const vec2 uvs[4] = {
  vec2(0, 0),
  vec2(0, 1),
  vec2(1, 0),
  vec2(1, 1)
};

const vec3 normals[6] = {
  vec3(-1, 0,  0),
  vec3(+1, 0,  0),
  vec3(0, +1,  0),
  vec3(0, -1,  0),
  vec3(0,  0, -1),
  vec3(0,  0, +1),
};

void main()
{
  int face   = (pack >>  0) & 0x007;
  int x      = (pack >>  3) & 0x01f;
  int y      = (pack >>  8) & 0x01f;
  int z      = (pack >> 13) & 0x01f;
  int corner = (pack >> 18) & 0x003;
  int type   = (pack >> 20) & 0x1ff;
  vec3 position = vec3(x, y, z);
  vec3 normal = normals[face];
  float cell_size = 1.f / float(tilemap_grid_size);
  int quad_x = type % tilemap_grid_size, quad_y = type / tilemap_grid_size;
  vec2 uv_base = vec2(quad_x * cell_size,
	                  quad_y * cell_size);
  vec2 uv = uvs[corner] * cell_size + uv_base;
  gl_Position = (projection * view * model) * vec4(position, 1.0);

  fragment_position = position;
  fragment_normal = normal;
  fragment_uv = uv;
}

