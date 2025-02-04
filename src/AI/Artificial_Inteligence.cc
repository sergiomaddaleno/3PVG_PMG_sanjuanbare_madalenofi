#include "AI/Artificial_Inteligence.h"


AI::AI()
{
}

AI::~AI()
{
}

#include <vector>
#include <cstdlib> // Para usar rand() y srand()
#include <ctime>   // Para usar time() en la semilla aleatoria

void AI::DrawGrid(ECSManager& ecs, int size, RenderSystem& rs, Shader shader)
{
    float cubeSize = 1.25f;
    std::vector<glm::vec3> gridPositions; 

    
    rs.createDirectionalLight(ecs, shader);

    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            TransformComponent t;

            t.position = { i * cubeSize, 0.0f, j * cubeSize };

            
            gridPositions.push_back(t.position);

           
            rs.createCube24v(ecs, shader, t, 0.5f);
        }
    }

    
    if (!gridPositions.empty()) {
        srand(static_cast<unsigned int>(time(0))); 

        
        int randomIndex_apple = rand() % gridPositions.size();

       
        glm::vec3 randomPosition_apple = gridPositions[randomIndex_apple];

       
        GenerateEntitiesAtPosition(ecs, rs, shader, randomPosition_apple,0);

        int randomIndex_snake = rand() % gridPositions.size();


        glm::vec3 randomPosition_snake = gridPositions[randomIndex_snake];


        GenerateEntitiesAtPosition(ecs, rs, shader, randomPosition_snake,1);
    }
}


void AI::GenerateEntitiesAtPosition(ECSManager& ecs, RenderSystem& rs, Shader shader, glm::vec3 position,int type)
{
    
    

    std::string url = "../assets/obj/apple.obj";
    std::string url_tex = "../assets/textures/apple";

    if (type == 0) {
        TransformComponent t;
        t.position.x = position.x;
        t.position.y = position.y - 1.0f;
        t.position.z = position.z;

        t.scale = glm::vec3(0.5f,0.5f,0.5f);
        rs.createMesh(ecs, shader,url,url_tex ,t);
    }
    else {
        TransformComponent t;
        t.position.x = position.x;
        t.position.y = position.y + 1.25f;
        t.position.z = position.z;
        rs.createCube24v(ecs, shader, t, 0.5f);

    }

}


