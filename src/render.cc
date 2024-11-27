
#include "..\include\render.h"
#include "..\include\mesh.h"
#include <../deps/linmath.h>
#include <memory> 
#define M_PI 3.14159265358979323846

Render::Render() : VAO(0), VBO(0), EBO(0) {}

std::optional<Render> Render::make()
{  
    glewInit();
   
    return std::optional<Render>();
}

GLuint Render::GetProgram()
{
    return program_;
}

ShaderBuffers Render::initShader(const char* vertex_shader_path, const char* fragment_shader_path,
    ShaderBuffers &shaders) {

    FILE* file = fopen(vertex_shader_path, "rb");
    long size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        shaders.vs = (char*)malloc(size + 1);
        if (shaders.vs) {
            fread(shaders.vs, sizeof(char), size, file);
            shaders.vs[size] = '\0';

            //printf("%s\n", shaders.vs);
        }
        fclose(file);
    }
    else {
        printf("Error in loading Vertex Shader\n");
    }

    file = fopen(fragment_shader_path, "rb");
    size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        shaders.fs = (char*)malloc(size + 1);
        if (shaders.fs) {
            fread(shaders.fs, sizeof(char), size, file);
            shaders.fs[size] = '\0';

            //printf("%s\n", shaders.fs);
        }
        fclose(file);
    }
    else {
        printf("Error in loading Fragment Shader\n");
    }

    return shaders;
}

void Render::SetupShader(ShaderBuffers &shaders) {

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shaders.vs, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shaders.fs, nullptr);
    glCompileShader(fragmentShader);

    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Render::RenderDrawTriangle(Triangle::PointsTriangle(&vertices)[3], TransformComponent tr)
{
    GLuint vertex_buffer;
    GLint vpos_location, vcol_location;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glUseProgram(program_);

    mat4x4_identity(model_);
    mat4x4_translate(model_, tr.position.x, tr.position.y, tr.position.z);
    mat4x4_rotate(model_, model_, tr.rotation.x, tr.rotation.y, tr.rotation.z, tr.angle);
    mat4x4_scale_aniso(model_, model_, tr.scale.x, tr.scale.y, tr.scale.z);

    mat4x4_identity(view_);
    mat4x4_translate(view_, 0.0f, 0.0f, -5.0f);

    mat4x4_perspective(projection_, (float)(55.0f * (M_PI / 180.0f)), (float)800 / 600, 0.1f, 100.0f);

    mat4x4_mul(mvp_, projection_, view_);
    mat4x4_mul(mvp_, mvp_, model_);
    glUniformMatrix4fv(glGetUniformLocation(program_, "MVP"), 1, GL_FALSE, (const GLfloat*)mvp_);

    vpos_location = glGetAttribLocation(program_, "vPos");
    vcol_location = glGetAttribLocation(program_, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDeleteBuffers(1, &vertex_buffer);
}

void Render::RenderMesh(std::unique_ptr<Mesh>& mesh, TransformComponent tr)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glUseProgram(program_);

    mat4x4_identity(model_);

    mat4x4_translate(model_, tr.position.x, tr.position.y, tr.position.z);
    mat4x4_rotate(model_, model_, tr.rotation.x, tr.rotation.y, tr.rotation.z, tr.angle);
    mat4x4_scale_aniso(model_, model_, tr.scale.x, tr.scale.y, tr.scale.z);

    mat4x4_identity(view_);
    mat4x4_translate(view_, 0.0f, -3.0f, -10.0f);

    mat4x4_perspective(projection_, (float)(55.0f * (M_PI / 180.0f)), 800.0f / 600.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, (const GLfloat*)model_);
    glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE, (const GLfloat*)view_);
    glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, (const GLfloat*)projection_);

    vec3 lightPos = { 1.0f, 1.0f, 1.0f };
    glUniform3fv(glGetUniformLocation(program_, "lightPos"), 1, (const GLfloat*)&lightPos);

    vec3 viewPos = { 1.0f, 1.0f, 3.0f };
    glUniform3fv(glGetUniformLocation(program_, "viewPos"), 1, (const GLfloat*)&viewPos);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Render::RenderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc)
{
    auto ri = rc.begin();
    auto ti = tc.begin();
    auto riend = rc.end();
    auto tiend = tc.end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            auto& mesh = render_comp->render_mesh;

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

            // Configuración de atributos de vértices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0); // Desvincular el VAO
            

            // Usar el programa de shaders
            glUseProgram(program_);

            // Calcular las matrices de transformación
            mat4x4_identity(model_);

            mat4x4_translate(model_, transform_comp->position.x, transform_comp->position.y, transform_comp->position.z);
            mat4x4_rotate(model_, model_, transform_comp->rotation.x, transform_comp->rotation.y, transform_comp->rotation.z, transform_comp->angle);
            mat4x4_scale_aniso(model_, model_, transform_comp->scale.x, transform_comp->scale.y, transform_comp->scale.z);

            mat4x4_identity(view_);
            mat4x4_translate(view_, 0.0f, -3.0f, -10.0f);

            mat4x4_perspective(projection_, (float)(55.0f * (M_PI / 180.0f)), 800.0f / 600.0f, 0.1f, 100.0f);

            // Pasar las matrices al shader
            glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, (const GLfloat*)model_);
            glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE, (const GLfloat*)view_);
            glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, (const GLfloat*)projection_);

            // Configurar las luces y la cámara
            vec3 lightPos = { 1.0f, 1.0f, 1.0f };
            glUniform3fv(glGetUniformLocation(program_, "lightPos"), 1, (const GLfloat*)&lightPos);

            vec3 viewPos = { 1.0f, 1.0f, 3.0f };
            glUniform3fv(glGetUniformLocation(program_, "viewPos"), 1, (const GLfloat*)&viewPos);

            glActiveTexture(GL_TEXTURE0); // Activa la textura 0
            glBindTexture(GL_TEXTURE_2D, mesh->GetTextureID());  // Vincula la textura cargada

            glUniform1i(glGetUniformLocation(program_, "texture1"), 0);

            // Renderizar la malla
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0); // Desvincular el VAO

            glUseProgram(0); // Desactivar el programa de shaders
        }
    }
}






