#ifndef __AI_H__
#define __AI_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/texture.h"
#include <memory>
#include "Render/render_system.h"
#include "Core/shader.h"
#include <queue>        
#include <vector>       
#include <functional>   
#include <unordered_map> 
#include <string>       
#include <algorithm>   

class ECSManager;

class AI {
public:

    AI();
    ~AI();

    void DrawGrid(ECSManager& ecs, int size, RenderSystem& rs, Shader shader);

    void GenerateEntitiesAtPosition(ECSManager& ecs, RenderSystem& rs, Shader shader, glm::vec3 position, int type);

    std::vector<glm::vec3> FindShortestPath(const glm::vec3& start, const glm::vec3& end, int gridSize, const std::vector<std::vector<bool>>& obstacles);

    bool IsValidCell(int x, int z, int size, const std::vector<std::vector<bool>>& obstacles);

};


#endif //__AI_H__