#ifndef COINS_H
#define COINS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mesh.h>

#include <deque>

float cradius = 10;
float csides = 20;
float coinAngSpeed = 1;
float coinRotate = 0;
float thickness = 2;
float coinSpeed = 200;
int coinCounter = 50;
int cc = 100;
int coinswon = 0;

class coin
{
    public:
        glm::vec3 pos;
        bool show;

    coin()
    {
        this->pos = glm::vec3(750.0f, 0.0f, 5.0f);
        this->pos.y = ((float)(rand() % 5) / (float)5 - 0.4) * 300;
        this->show = true;
    }

    bool checkCol(glm::vec3 &playerPos, float px, float py)
    {
        if(this->pos.x < px + playerPos.x + 2 * cradius && this->pos.x > -px + playerPos.x - 2 * cradius)
        {
            float range = 0;
            if(this->pos.x > px + playerPos.x)
            {
                range = sqrt(cradius * cradius - (this->pos.x - px) * (this->pos.x - px));
            }
            else if(this->pos.x < -px + playerPos.x)
            {
                range = sqrt(cradius * cradius - (-px - this->pos.x) * (-px - this->pos.x));
            }
            else
            {
                range = cradius;
            }
            if(this->pos.y + range > py + playerPos.y &&  this->pos.y - range < py + playerPos.y)
            {
                return true;
            }
            if(this->pos.y + range > -py + playerPos.y &&  this->pos.y - range < -py + playerPos.y)
            {
                return true;
            }
            if(this->pos.y + range < py + playerPos.y &&  this->pos.y - range > -py + playerPos.y)
            {
                return true;
            }
        }
        return false;
    }
};

std::deque<coin> coinQueue;

#endif