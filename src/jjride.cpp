// OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Headers
#include <shader.h>
#include <mesh.h>
#include <obstacle.h>

// C++ Headers
#include <iostream>
// #include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

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

            if(obsQueue[i].checkCol(playerPos, px, py))
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
