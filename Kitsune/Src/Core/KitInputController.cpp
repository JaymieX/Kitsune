#include "KitInputController.h"

#include <algorithm>

namespace Kitsune
{
    void KitInputController::MoveXZ(GLFWwindow* window, const float dt, KitGameObject& game_object)
    {
        glm::vec3 rotation(0.f);
        if(glfwGetKey(window, keys.look_right) == GLFW_PRESS)
        {
            rotation.y++;
        }

        if(glfwGetKey(window, keys.look_left) == GLFW_PRESS)
        {
            rotation.y--;
        }

        if(glfwGetKey(window, keys.look_up) == GLFW_PRESS)
        {
            rotation.x++;
        }

        if(glfwGetKey(window, keys.look_down) == GLFW_PRESS)
        {
            rotation.x--;
        }

        if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
        {
            game_object.transform.rotation += rotate_speed * dt *glm::normalize(rotation);
        }

        game_object.transform.rotation.x = glm::clamp(game_object.transform.rotation.x, -1.5f, 1.5f);
        game_object.transform.rotation.y = glm::mod(game_object.transform.rotation.y, glm::two_pi<float>());

        float yaw = game_object.transform.rotation.y;
        glm::vec3 front(sin(yaw), 0.f, cos(yaw));
        glm::vec3 right(front.z, 0.f, -front.x);
        glm::vec3 up(0.f, -1.f, 0.f);

        glm::vec3 move_dir(0.f);

        if(glfwGetKey(window, keys.move_forward) == GLFW_PRESS)
        {
            move_dir += front;
        }

        if(glfwGetKey(window, keys.move_backward) == GLFW_PRESS)
        {
            move_dir -= front;
        }

        if(glfwGetKey(window, keys.move_right) == GLFW_PRESS)
        {
            move_dir += right;
        }

        if(glfwGetKey(window, keys.move_left) == GLFW_PRESS)
        {
            move_dir -= right;
        }

        if(glfwGetKey(window, keys.move_up) == GLFW_PRESS)
        {
            move_dir += up;
        }

        if(glfwGetKey(window, keys.move_down) == GLFW_PRESS)
        {
            move_dir -= up;
        }

        if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon())
        {
            game_object.transform.translation += move_speed * dt *glm::normalize(move_dir);
        }
    }
}
