#version 420

uniform sampler2D texture0;
uniform vec3 ambient_light;
uniform int tilemap_grid_size;
in vec3 fragment_position;
in vec2 fragment_uv;
in vec3 fragment_normal;
in vec2 fragment_uvstretch;
in vec2 fragment_uvbase;
out vec3 fragment_color;

void main(){
  float cell_size = 1.f / float(tilemap_grid_size);
  vec3 light_direction = normalize(ambient_light);
  float diff = max(dot(fragment_normal, light_direction), 0.3);
  vec2 true_uv = fragment_uv;
  true_uv.x = abs(true_uv.x - int(true_uv.x));
  true_uv.y = abs(true_uv.y - int(true_uv.y));
  true_uv *= cell_size;
  fragment_color = (diff * vec4(1.0, 1.0, 1.0, 1.0) * texture(texture0, fragment_uvbase + true_uv)).rgb;
  //fragment_color = vec3(true_uv, 0);
}
