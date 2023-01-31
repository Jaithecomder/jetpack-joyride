#ifndef COINS_H
#define COINS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mesh.h>

#include <deque>

float cradius = 10;
float csides = 100;
float coinAngSpeed = 5;

class coin
{
    public:
        glm::vec3 pos;
        bool show;

    coin()
    {
        this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
        this->show = true;
    }

    bool checkCol(glm::vec3 &playerPos, float px, float py)
    {
        return false;
    }
};

std::deque<coin> coinQueue;

#endif