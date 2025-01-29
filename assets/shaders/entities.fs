#version 330 core

uniform sampler2D texture_;
uniform sampler2D shadowMap;  // Mapa de sombras
uniform int u_number_lights;
uniform vec3 u_camera_position;
uniform vec3 u_ambient;
uniform bool u_fog;

vec3 skyColour = vec3(0.5, 0.5, 0.5);
in float visibility;

out vec4 FragColor;

in vec3 normal;
in vec3 pos;
in vec2 uv;
in vec3 FragPos;
in vec4 FragPosLightSpace;  // Posición en el espacio de la luz

const int kMaxLights = 50;

struct LightInfo {
    int type_;
    vec3 pos_;
    vec3 dir_;
    vec3 diff_color_;
    vec3 spec_color_;
    vec3 light_color_;
    vec3 ambient_color_;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle_;
};

uniform LightInfo u_light[kMaxLights];

// Función para calcular la sombra
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Transformar la posición del fragmento a espacio de luz
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;  // Dividir por w (perspectiva)
    projCoords = projCoords * 0.5 + 0.5; // Transformar a rango [0, 1]

    // Muestrear el mapa de sombras
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Desplazamiento para evitar el acne de sombra
    float bias = 0.005;

    // Determinar si el fragmento está en sombra
    return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}

// Función para luz puntual
vec3 PointLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);

    vec3 specular = specStrength * lc.light_color_ * attenuation;
    return diffuse + specular;
}

// Función para luz direccional
vec3 DirectionalLight(LightInfo lc) {
    vec3 light_dir = normalize(-lc.dir_);

    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * lc.light_color_;

    vec3 view_dir = normalize(u_camera_position - FragPos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), lc.shininess_);
    vec3 specular = spec * lc.spec_color_;

    vec3 ambient = lc.ambient_color_ * lc.light_color_;
    return ambient + diffuse + specular;
}

// Función para luz spot
vec3 SpotLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float cutOff = cos(radians(lc.spot_angle_));
    float theta = dot(lightDir, normalize(-lc.dir_));

    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    if (theta > cutOff) {
        float diffuseStrength = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation;

        vec3 viewDir = normalize(u_camera_position - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);
        vec3 specular = specStrength * lc.light_color_ * attenuation;

        return diffuse + specular;
    }
    else {
        return lc.ambient_color_ * u_ambient;
    }
}

void main()
{
    vec3 final_light = u_ambient;

    for (int i = 0; i < u_number_lights; ++i) {
        float shadow = ShadowCalculation(FragPosLightSpace); // Verificar si está en sombra

        if (u_light[i].type_ == 0) {
            final_light += DirectionalLight(u_light[i]) * shadow; // Aplicar sombra
        }

        if (u_light[i].type_ == 1) {
            final_light += SpotLight(u_light[i]) * shadow;  // Aplicar sombra
        }

        if (u_light[i].type_ == 2) {
            final_light += PointLight(u_light[i]) * shadow;  // Aplicar sombra
        }
    }

    vec4 texture_color = texture(texture_, uv);
    vec3 texture_and_light = texture_color.rgb * final_light;

    FragColor = vec4(texture_and_light, 1.0);

    if (u_fog) {
        FragColor = mix(vec4(skyColour, 1.0), FragColor, visibility);
    }
}