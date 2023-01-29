#ifndef OBS_H
#define OBS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <deque>

float obsLength = 125;
unsigned obsCount = 100;
unsigned int spawnCounter = 0;
unsigned int Level = 1;
class obstacle
{
    public:
        glm::vec3 obsPos;
        float obsRotate;
        float obsSpeed;
        float obsAngSpeed;

        obstacle()
        {
            this->obsPos = glm::vec3(750.0f, 0.0f,  0.0f);
            this->obsRotate = rand() % 180;
            this->obsPos.y = ((float)(rand() % 5) / (float)5 - 0.4) * 300;
            // this->obsPos.y = -0.6;
            this->obsSpeed = -200;
            this->obsAngSpeed = 0;
        }

        bool checkCol(glm::vec3 &playerPos, float px, float py)
        {
            glm::mat4 obsPR = glm::mat4(1.0f);
            obsPR = glm::translate(obsPR, this->obsPos);
            obsPR = glm::rotate(obsPR, glm::radians(this->obsRotate), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::vec4 p1 = glm::vec4(0.5f * obsLength, 0.025f, 0.0f, 1.0f);
            glm::vec4 p2 = glm::vec4(-0.5f * obsLength, 0.025f, 0.0f, 1.0f);
            glm::vec4 p3 = glm::vec4(-0.5f * obsLength, -0.025f, 0.0f, 1.0f);
            glm::vec4 p4 = glm::vec4(0.5f * obsLength, -0.025f, 0.0f, 1.0f);

            p1 = obsPR * p1;
            p2 = obsPR * p2;
            p3 = obsPR * p3;
            p4 = obsPR * p4;

            float low, high;

            unsigned int p1m = 1;
            unsigned int p2m = 1;
            unsigned int p3m = 1;
            unsigned int p4m = 1;

            if(p1.x <= px + playerPos.x)
            {
                p1m--;
            }
            if(p1.x < -px + playerPos.x)
            {
                p1m--;
            }

            if(p2.x <= px + playerPos.x)
            {
                p2m--;
            }
            if(p2.x < -px + playerPos.x)
            {
                p2m--;
            }

            if(p3.x <= px + playerPos.x)
            {
                p3m--;
            }
            if(p3.x < -px + playerPos.x)
            {
                p3m--;
            }

            if(p4.x <= px + playerPos.x)
            {
                p4m--;
            }
            if(p4.x < -px + playerPos.x)
            {
                p4m--;
            }

            if(this->obsRotate < 90.0f)
            {
                if((p2m == 1) || (p4m == -1))
                {
                    return false;
                }

                bool p2in = (p2.x < px + playerPos.x) & (p2.x > -px + playerPos.x);
                bool p4in = (p4.x < px + playerPos.x) & (p4.x > -px + playerPos.x);
                if(p1m == 0)
                {
                    high = p1.y;
                }
                else
                {
                    float m, c;
                    if(p2m * p1m != 1)
                    {
                        c = (p1.x * p2.y - p2.x * p1.y) / (p1.x - p2.x);
                        m = (p1.y - p2.y) / (p1.x - p2.x);
                        high = m * (px + playerPos.x) + c;
                    }
                    else if(p4m * p1m != 1)
                    {
                        c = (p1.x * p4.y - p4.x * p1.y) / (p1.x - p4.x);
                        m = (p1.y - p4.y) / (p1.x - p4.x);
                        high = m * (-px + playerPos.x) + c;
                    }
                }
                if(high <= py + playerPos.y && high >= -py + playerPos.y)
                {
                    return true;
                }

                if(p3m == 0)
                {
                    low = p3.y;
                }
                else
                {
                    float m, c;
                    if(p2m * p3m != 1)
                    {
                        c = (p3.x * p2.y - p2.x * p3.y) / (p3.x - p2.x);
                        m = (p3.y - p2.y) / (p3.x - p2.x);
                        low = m * (px + playerPos.x) + c;
                    }
                    else if(p4m * p3m != 1)
                    {
                        c = (p3.x * p4.y - p4.x * p3.y) / (p3.x - p4.x);
                        m = (p3.y - p4.y) / (p3.x - p4.x);
                        low = m * (-px + playerPos.x) + c;
                    }
                }
                if(low <= py + playerPos.y && low >= -py + playerPos.y)
                {
                    return true;
                }
            }
            else
            {
                if((p1m == 1) || (p3m == -1))
                {
                    return false;
                }

                if(p4m == 0)
                {
                    high = p4.y;
                }
                else
                {
                    float m, c;
                    if(p1m * p4m != 1)
                    {
                        c = (p4.x * p1.y - p1.x * p4.y) / (p4.x - p1.x);
                        m = (p4.y - p1.y) / (p4.x - p1.x);
                        high = m * (px + playerPos.x) + c;
                    }
                    else if(p3m * p4m != 1)
                    {
                        c = (p4.x * p3.y - p3.x * p4.y) / (p4.x - p3.x);
                        m = (p4.y - p3.y) / (p4.x - p3.x);
                        high = m * (-px + playerPos.x) + c;

                    }
                }
                if(high <= py + playerPos.y && high >= -py + playerPos.y)
                {
                    return true;
                }

                if(p2m == 0)
                {
                    low = p2.y;
                }
                else
                {
                    float m, c;
                    if(p1m * p2m != 1)
                    {
                        c = (p2.x * p1.y - p1.x * p2.y) / (p2.x - p1.x);
                        m = (p2.y - p1.y) / (p2.x - p1.x);
                        low = m * (px + playerPos.x) + c;
                    }
                    else if(p3m * p2m != 1)
                    {
                        c = (p2.x * p3.y - p3.x * p2.y) / (p2.x - p3.x);
                        m = (p2.y - p3.y) / (p2.x - p3.x);
                        low = m * (-px + playerPos.x) + c;
                    }
                }
                if(low <= py + playerPos.y && low >= -py + playerPos.y)
                {
                    return true;
                }
            }

            if(high >= py + playerPos.y && low <= -py + playerPos.y)
            {
                return true;
            }

            return false;
        }
};

std::deque<obstacle> obsQueue;

#endif