#version 330 core

uniform float u_time;
uniform vec2 u_resolution;
uniform vec3 u_ray_origin; 

in vec2 vUv;
out vec4 FragColor;

#define MAX_STEPS 100
//#define MAX_DIST 100.0
//#define SURFACE_DIST 0.01

const float MARCH_SIZE = 0.08; 

float sdSphere(vec3 p, float radius) {
    return length(p) - radius;
}

float scene(vec3 p) {
    float distance = sdSphere(p, 1.0);
    return -distance;
}

vec4 raymarch(vec3 ro, vec3 rd) {
    
    float depth = 0.0;
    vec3 p = u_ray_origin + depth * rd;

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

  gl_FragColor = vec4(color, 1.0);
}
