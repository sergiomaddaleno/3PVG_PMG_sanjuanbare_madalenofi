#version 330 core

uniform float u_time;
uniform vec2 u_resolution;
uniform vec3 u_camera_pos; 
uniform sampler2D u_noise_2D;
uniform sampler3D u_noise_3D;

out vec4 FragColor;
in vec3 v_ray_dir;

#define MAX_STEPS 100
const float MARCH_SIZE = 0.08; 

float sdSphere(vec3 p, float radius) {
    return length(p) - radius;
}

float noise(vec3 x ) {
  vec3 p = floor(x);
  vec3 f = fract(x);
  f = f * f * (3. - 2. * f);

  vec2 uv = (p.xy + vec2(37.0, 239.0) * p.z) + f.xy;
  vec2 tex = textureLod(u_noise_2D,(uv + 0.5) / 256.0, 0.0).yx;

  return mix( tex.x, tex.y, f.z ) * 2.0 - 1.0;
}

float fbm(vec3 p) {
  vec3 q = p + u_time * 0.5 * vec3(1.0, -0.2, -1.0);
  float g = noise(q);

  float f = 0.0;
  float scale = 0.5;
  float factor = 2.02;

  for (int i = 0; i < 6; i++) {
      f += scale * noise(q);
      q *= factor;
      factor += 0.21;
      scale *= 0.5;
  }

  return f;
}

float scene(vec3 p) {
  float distance = sdSphere(p, 1.0);
  float f = fbm(p);

  return -distance + f;
}

vec4 raymarch(vec3 ro, vec3 rd) {
    
    float depth = 0.0;
    vec3 p = u_camera_pos + depth * rd;

    vec4 res = vec4(0.0);

    for(int i = 0; i < MAX_STEPS; i++) {
        float density = scene(p);

        if(density > 0) {
            vec4 color = vec4(mix(vec3(1.0,1.0,1.0), vec3(0.0, 0.0, 0.0), density), density );
            color.rgb *= color.a;
            res += color*(1.0-res.a);
        }
        depth += MARCH_SIZE;
        p = ro + depth * rd;
    }

    return res;
}

void main() {
  vec2 uv = gl_FragCoord.xy / u_resolution.xy;
  uv -= 0.5;
  uv.x *= u_resolution.x / u_resolution.y;

  vec3 ray_origin = vec3(0.0, 0.0, 5.0);
  vec3 ray_dir = normalize(vec3(uv, -1.0));

  vec3 color = vec3(0.0);
  vec4 res = raymarch(ray_origin, ray_dir);
  color = res.rgb;

  FragColor = vec4(color, 1.0);
}
