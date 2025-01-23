#pragma once
#include <GLFW/glfw3.h>

#include "Scene/KitGameObject.h"

namespace Kitsune
{
    class KitInputController
    {
    public:
        struct KitKepMapping
        {
            int move_left     = GLFW_KEY_A;
            int move_right    = GLFW_KEY_D;
            int move_forward  = GLFW_KEY_W;
            int move_backward = GLFW_KEY_S;
            int move_up       = GLFW_KEY_E;
            int move_down     = GLFW_KEY_Q;
            int look_left     = GLFW_KEY_LEFT;
            int look_right    = GLFW_KEY_RIGHT;
            int look_up       = GLFW_KEY_UP;
            int look_down     = GLFW_KEY_DOWN;
        };

        KitKepMapping keys;
        float move_speed = 3.f;
        float rotate_speed = 1.5f;

        void MoveXZ(GLFWwindow* window, const float dt, KitGameObject& game_object);
    };
}
