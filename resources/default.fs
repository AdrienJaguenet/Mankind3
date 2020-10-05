#version 420

uniform sampler2D texture0;
uniform vec3 ambient_light;
in vec3 fragment_position;
in vec2 fragment_uv;
in vec3 fragment_normal;
out vec3 fragment_color;

void main(){
  vec3 light_direction = normalize(ambient_light);
  float diff = max(dot(fragment_normal, light_direction), 0.3);
  fragment_color = (diff * vec4(1.0, 1.0, 1.0, 1.0) * texture(texture0, fragment_uv)).rgb;
}
