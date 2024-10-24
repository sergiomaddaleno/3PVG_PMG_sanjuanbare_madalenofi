
#include "window.h"
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\job_sistem.h"
#include <iostream>
#include <memory>
#include <future>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <optional>
#include <string>

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"        
"layout(location = 1) in vec3 aNormal;\n"    
"layout(location = 2) in vec2 aTexCoord;\n"   
"out vec2 TexCoord;\n"                        
"out vec3 Normal;\n"                          
"out vec3 FragPos;\n"                        
"uniform mat4 model;\n"                       
"uniform mat4 view;\n"                       
"uniform mat4 projection;\n"                  
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n" 
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n" 
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    TexCoord = aTexCoord;\n"                  
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"                       
"in vec2 TexCoord;\n"                         
"in vec3 Normal;\n"                           
"in vec3 FragPos;\n"                           
"uniform vec3 lightPos;\n"                    
"uniform vec3 viewPos;\n"                      
"void main()\n"
"{\n"
    
"    vec3 norm = normalize(Normal);\n"
    
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
   
"    float diff = max(dot(norm, lightDir), 0.0);\n"
    
"    FragColor = vec4(diff * vec3(1.0, 1.0, 1.0), 1.0); // Blanco\n"
"}\n";

void printf_() {
	std::cout << "Tarea 1 ejecutada\n";
}


int main(int, char**) {
    int currentMeshIndex = 0;
    bool loadingNewMesh = false;

    std::vector<std::string> meshFiles = {
        "../assets/SM_Suzanne.obj",
        "../assets/SM_Teapot.obj",
        "../assets/SM_Cube.obj"
    };

    Mesh mesh_;

    // Cargar la primera malla al inicio
    if (!mesh_.LoadOBj(meshFiles[currentMeshIndex])) {
        std::cerr << "Error al cargar la malla inicial: " << meshFiles[currentMeshIndex] << std::endl;
        return -1;
    }

    glfwInit();
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Render r;
    auto render = r.make();

    bool done = false;
    JobSystem jb;

    auto lastChangeTime = std::chrono::steady_clock::now(); // Inicializa el temporizador
    std::future<std::unique_ptr<Mesh>> monetemesh; // Declaración del futuro

    while (!done) {
        // Comprobar si han pasado 3 segundos
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastChangeTime).count() >= 3 && !loadingNewMesh) {
            // Cambiar al siguiente índice de malla cíclicamente
            currentMeshIndex = (currentMeshIndex + 1) % meshFiles.size();
            std::string nextMeshFile = meshFiles[currentMeshIndex];

            std::cout << "Cambiando a la malla: " << nextMeshFile << std::endl; // Mensaje de depuración

            // Cargar la nueva malla en segundo plano
            loadingNewMesh = true;
            monetemesh = jb.Add_Task([nextMeshFile]() {
                std::cout << "Intentando cargar la malla: " << nextMeshFile << std::endl; // Mensaje de depuración
                std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();
                if (newMesh->LoadOBj(nextMeshFile)) {
                    std::cout << "Malla cargada exitosamente: " << nextMeshFile << std::endl; // Mensaje de depuración
                    return newMesh; // Devuelve el nuevo Mesh si se carga correctamente
                }
                else {
                    std::cerr << "Error al cargar la malla: " << nextMeshFile << std::endl;
                    return std::unique_ptr<Mesh>(nullptr); // Devuelve nullptr si hay un error
                }
                });

            lastChangeTime = currentTime; // Reiniciar el temporizador
        }

        // Verificar si la tarea de carga ha terminado
        if (loadingNewMesh && monetemesh.valid()) {
            //if (monetemesh.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                std::unique_ptr<Mesh> loadedMesh = monetemesh.get();
                if (loadedMesh) {
                    mesh_ = std::move(*loadedMesh); // Reemplazar la malla actual
                }
                loadingNewMesh = false; // Resetea la bandera
            //}
        }

        // Renderizar la malla actual
        r.RenderMesh(vertex_shader_text, fragment_shader_text, mesh_);

        // Actualizar la ventana
        window->UpdateWindow();

        // Salir si se presiona el botón de cerrar ventana
        if (window->closePressed()) done = true;
    }

    return 0;
}
