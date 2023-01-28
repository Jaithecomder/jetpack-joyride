// OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Headers
#include <shader.h>
#include <mesh.h>

// C++ Headers
#include <iostream>
// #include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <deque>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 500;

// camera
glm::vec3 camPos   = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// player
float px = 0.075;
float py = 0.1;
glm::vec3 playerPos = glm::vec3(-1.0f, 0.0f,  0.0f);
unsigned int pressTime = 0;
unsigned int releaseTime = 0;
float playerSpeed = 0;
float upacc = 0.1;
float downacc = -0.15;
float uplim = 0.6;
float downlim = -0.6;

// platform
float platSpeed = 0;

// obstacle
float obsLength = 0.5f;
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
            this->obsPos = glm::vec3(2.0f, 0.0f,  0.0f);
            this->obsRotate = rand() % 180;
            this->obsPos.y = (float)(rand() % 5) / (float)5 - 0.4;
            // this->obsPos.y = -0.6;
            this->obsSpeed = -1;
            this->obsAngSpeed = 0;
        }

        bool checkCol()
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

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        releaseTime = 0;
        pressTime++;
        if(playerPos.y < uplim)
        {
            playerSpeed = playerSpeed + upacc * deltaTime;
            playerPos += glm::vec3(0.0f, 1.0f, 0.0f) * playerSpeed;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        pressTime = 0;
        releaseTime++;
        if(playerPos.y > downlim)
        {
            playerSpeed = playerSpeed + downacc * deltaTime;
            playerPos += glm::vec3(0.0f, 1.0f, 0.0f) * playerSpeed;
        }
    }

}

int main(int argc, char ** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 500, "Assignment 1", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "couldnt create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader myshader("../src/vertex.shader", "../src/fragment.shader");
    
    std::vector<float> playerVert{
        px, py, 0.0f, 1.0f, 0.0f, 0.0f,
        -px, py, 0.0f, 1.0f, 0.0f, 0.0f,
        -px, -py, 0.0f, 1.0f, 0.0f, 0.0f,
        px, -py, 0.0f, 1.0f, 0.0f, 0.0f
    };
    std::vector<unsigned int> playerInd{
        0, 1, 2,
        0, 2, 3
    };

    Mesh player(playerVert, playerInd);

    std::vector<float> platformVert{
        -1.5f, -0.7f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.5f, -0.9f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.5f, -0.9f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.5f, -0.7f, 0.0f, 0.0f, 1.0f, 0.0f,

        -1.5f, 0.7f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.5f, 0.9f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.5f, 0.9f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.5f, 0.7f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    std::vector<unsigned int> platformInd{
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7
    };

    Mesh platforms(platformVert, platformInd);

    std::vector<float> obstacleVert{
        0.5f * obsLength, 0.025f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f * obsLength, 0.025f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f * obsLength, -0.025f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f * obsLength, -0.025f, 0.0f, 1.0f, 1.0f, 0.0f,
    };

    std::vector<unsigned int> obstacleInd{
        0, 1, 2,
        0, 2, 3,
    };

    Mesh obstacles(obstacleVert, obstacleInd);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myshader.use();
        myshader.setMat4("projection", projection);

        // draw player
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        if(playerPos.y > uplim)
        {
            playerPos.y = uplim;
            playerSpeed = 0;
        }

        if(playerPos.y < downlim)
        {
            playerPos.y = downlim;
            playerSpeed = 0;
        }

        model = glm::translate(model, playerPos);
        view = glm::lookAt(camPos, camPos + camFront, camUp);

        myshader.setMat4("view", view);
        myshader.setMat4("model", model);

        player.drawObject();

        //draw platforms
        view = glm::mat4(1.0f);
        model = glm::mat4(1.0f);

        view = glm::lookAt(camPos, camPos + camFront, camUp);

        myshader.setMat4("view", view);
        myshader.setMat4("model", model);

        platforms.drawObject();

        glBindVertexArray(0);

        //draw obstacles

        if(spawnCounter == 0)
        {
            spawnCounter = obsCount;
            obstacle zapper;
            obsQueue.push_back(zapper);
        }
        else
        {
            spawnCounter -= Level;
        }

        // if(spawnCounter == 0)
        // {
        //     spawnCounter++;
        //     obstacle zapper;
        //     obsQueue.push_back(zapper);
        // }

        for(int i = 0; i<obsQueue.size(); i++)
        {
            if(obsQueue[i].obsPos.x < -2.0)
            {
                obsQueue.pop_front();
                i--;
                continue;
            }

            if(obsQueue[i].checkCol())
            {
                glfwSetWindowShouldClose(window, true);
            }

            view = glm::mat4(1.0f);
            model = glm::mat4(1.0f);

            obsQueue[i].obsPos.x += obsQueue[i].obsSpeed * deltaTime;

            model = glm::translate(model, obsQueue[i].obsPos);
            model = glm::rotate(model, glm::radians(obsQueue[i].obsRotate), glm::vec3(0.0f, 0.0f, 1.0f));
            view = glm::lookAt(camPos, camPos + camFront, camUp);

            myshader.setMat4("view", view);
            myshader.setMat4("model", model);

            obstacles.drawObject();

            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
