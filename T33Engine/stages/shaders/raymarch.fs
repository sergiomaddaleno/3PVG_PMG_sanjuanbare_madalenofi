#version 330 core

uniform float u_time;
uniform vec2 u_resolution;
uniform sampler2D u_noise_2D;

uniform vec3 u_cube_scale;
uniform vec3 u_light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 u_absorption = vec3(0.2, 0.2, 0.2);
uniform float u_speed = 1.0f;
uniform float u_march_size = 0.04;
uniform int u_max_steps = 200;
uniform int u_max_steps_lights = 6;
uniform float u_amplitude = 2.0;
uniform float u_frequency = 0.5;
uniform vec3 u_cloud_direction = vec3(0.1, 0.2, -0.3);
uniform vec3 u_sun_dir = vec3(0.0, -1.0, 0.0);

in vec3 world_pos;
in vec3 ray_origin;
in vec3 ray_direction;
out vec4 FragColor;

float noise(vec3 x ) {
  vec3 p = floor(x);
  vec3 f = fract(x);
  f = f * f * (3.0 - 2.0 * f);

  vec2 uv = (p.xy + vec2(37.0, 239.0) * p.z) + f.xy;
  vec2 tex = textureLod(u_noise_2D,(uv + 0.5) / 256.0, 0.0).yx;

  return mix( tex.x, tex.y, f.z ) * 2.0 - 1.0;
}

float fbm(vec3 p) {
  float f = 0.0;
  float amp = u_amplitude;
  float freq = u_frequency;

  for (int i = 0; i < 5; i++) {
      f += amp * noise(p * freq);
      freq *= 2.0;
      amp *= 0.5;
  }

  return clamp(f, 0.0, 1.0);
}

vec3 BeersLaw(float dist, vec3 absorption) {
    return exp(-dist * absorption);
}

bool cubeIntersection(vec3 ro, vec3 rd, out float tmin, out float tmax) {
    vec3 box_min = vec3(-u_cube_scale * 0.5);
    vec3 box_max = vec3( u_cube_scale * 0.5);

    vec3 inv_dir = 1.0 / rd;
    vec3 t0 = (box_min - ro) * inv_dir;
    vec3 t1 = (box_max - ro) * inv_dir;

    vec3 tmin_vec = min(t0, t1);
    vec3 tmax_vec = max(t0, t1);

    tmin = max(max(tmin_vec.x, tmin_vec.y), tmin_vec.z);
    tmax = min(min(tmax_vec.x, tmax_vec.y), tmax_vec.z);

    return tmax >= max(tmin, 0.0);
}

vec3 lightmarch(vec3 position, vec3 ray_direction){
  float totalDensity = 0.0;

  for (int step = 0; step < u_max_steps_lights; step++) {
      position += u_sun_dir * u_march_size;

      float lightSample = fbm(position * 2.0 + (u_time * u_speed) * u_cloud_direction);
      lightSample = max(0.0, lightSample - 0.3);

      totalDensity += lightSample;
  }

  return BeersLaw(totalDensity * u_march_size, u_absorption);
}

vec4 raymarch(vec3 ro, vec3 rd, float tmin, float tmax) {
    vec4 accum = vec4(0.0);
    float t = tmin;

    for (int i = 0; i < u_max_steps && t < tmax; ++i) {
        vec3 pos = ro + t * rd;
        float density = fbm(pos * 2.0 + (u_time * u_speed) * u_cloud_direction);
        density = max(0.0, density - 0.3); 

        vec3 transmittance = BeersLaw(density * u_march_size, u_absorption);
        vec3 light = lightmarch(pos, u_sun_dir);
        vec3 color = u_light_color * density * light;

        accum.rgb += (1.0 - accum.a) * color * transmittance;
        accum.a += (1.0 - accum.a) * density * 0.2;

        t += u_march_size;
    }

    return accum;
}

void main() {
    float tmin, tmax;

    vec3 rd = normalize(ray_direction);
    if (cubeIntersection(ray_origin, rd, tmin, tmax)) {
        FragColor = raymarch(ray_origin, rd, tmin, tmax);
        return;
    }

    FragColor = vec4(0.0);
}