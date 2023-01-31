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
#include <text.h>
#include <coins.h>

// C++ Headers
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 500;

// camera
glm::vec3 camPos   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// player
float px = 20;
float py = 40;
glm::vec3 playerPos = glm::vec3(-300.0f, 0.0f,  0.0f);
unsigned int pressTime = 0;
unsigned int releaseTime = 0;
float playerSpeed = 0;
float upacc = 10;
float downacc = -15;
float uplim = 160;
float downlim = -160;
float distance = 0;

// window
int windowWidth;
int windowHeight;

// platform
float platSpeed = 0;

// game
bool over = false;
bool win = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    Shader textshader("../src/vertex.shader", "../src/textfrag.shader");
    Shader playershader("../src/vertex.shader", "../src/playerfrag.shader");
    Shader obsshader("../src/vertex.shader", "../src/obsfrag.shader");

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../fonts/Poppins-ExtraLight.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; 
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 192);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    charInit(face);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    textInit();

    std::vector<float> playerVert{
        px, py, 0.0f, 1.0f, 1.0f,
        -px, py, 0.0f, 0.0f, 1.0f,
        -px, -py, 0.0f, 0.0f, 0.0f,
        px, -py, 0.0f, 1.0f, 0.0f
    };
    std::vector<unsigned int> playerInd{
        0, 1, 2,
        0, 2, 3
    };

    Mesh player(playerVert, playerInd);

    std::vector<float> bgVert{
        450.0f, 250.0f, -50.0f, 1.0f, 1.0f,
        -450.0f, 250.0f, -50.0f, 0.0f, 1.0f,
        -450.0f, -250.0f, -50.0f, 0.0f, 0.0f,
        450.0f, -250.0f, -50.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> bgInd{
        0, 1, 2,
        0, 2, 3
    };

    Mesh background(bgVert, bgInd);

    std::vector<float> platformVert{
        450.0f, -200.0f, 0.0f, 1.0f, 1.0f,
        -450.0f, -200.0f, 0.0f, 0.0f, 1.0f,
        -450.0f, -250.0f, 0.0f, 0.0f, 0.0f,
        450.0f, -250.0f, 0.0f, 1.0f, 0.0f,

        450.0f, 250.0f, 0.0f, 1.0f, 1.0f,
        -450.0f, 250.0f, 0.0f, 0.0f, 1.0f,
        -450.0f, 200.0f, 0.0f, 0.0f, 0.0f,
        450.0f, 200.0f, 0.0f, 1.0f, 0.0f,
    };
    std::vector<unsigned int> platformInd{
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7
    };

    Mesh platforms(platformVert, platformInd);

    std::vector<float> obstacleVert{
        0.5f * obsLength, 7.5f, 0.0f, 1.0f, 1.0f,
        -0.5f * obsLength, 7.5f, 0.0f, 0.0f, 1.0f,
        -0.5f * obsLength, -7.5f, 0.0f, 0.0f, 0.0f,
        0.5f * obsLength, -7.5f, 0.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> obstacleInd{
        0, 1, 2,
        0, 2, 3,
    };

    Mesh obstacles(obstacleVert, obstacleInd);

    std::vector<float> cvert;
    std::vector<unsigned int> cfind, csind;

    for(int i = 0; i < csides; i++)
    {
        float angle = i * (float)360 / (float)csides;
        float x = glm::sin(glm::radians(angle));
        float y = -1 * glm::cos(glm::radians(angle));
        float z = thickness;
        cvert.push_back(cradius * x);
        cvert.push_back(cradius * y);
        cvert.push_back(z);
        cvert.push_back(x);
        cvert.push_back(y);

        cvert.push_back(cradius * x);
        cvert.push_back(cradius * y);
        cvert.push_back(-z);
        cvert.push_back(x);
        cvert.push_back(y);
    }

    for(int i = 0; i < csides - 2; i++)
    {
        unsigned int x = 2 * (i + 1);
        unsigned int y = 2 * (i + 2);
        cfind.push_back(0);
        cfind.push_back(x);
        cfind.push_back(y);
        cfind.push_back(1);
        cfind.push_back(x + 1);
        cfind.push_back(y + 1);
    }

    Mesh coinFaces(cvert, cfind);

    for(int i = 0; i < 2 * csides - 2; i++)
    {
        unsigned int x = i + 1;
        unsigned int y = i + 2;
        csind.push_back(i);
        csind.push_back(x);
        csind.push_back(y);
    }

    csind.push_back(2 * csides - 2);
    csind.push_back(2 * csides - 1);
    csind.push_back(0);
    csind.push_back(2 * csides - 1);
    csind.push_back(0);
    csind.push_back(1);

    Mesh coinSides(cvert, csind);

    while(!glfwWindowShouldClose(window))
    {
        float ratio = (float)windowWidth / windowHeight;
        float height = 250.f;
        float width = height * ratio;
        glm::mat4 projection = glm::ortho(-width, width, -height, height, -100.0f, 100.0f);
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(distance > ldist)
        {
            if(Level < 3)
            {
                Level++;
                distance = 0;
            }
            else
            {
                win = true;
            }
        }

        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        myshader.use();
        view = glm::lookAt(camPos, camPos + camFront, camUp);
        myshader.setMat4("projection", projection);

        // draw background----------------------------------------------------------------
        model = glm::mat4(1.0f);

        myshader.setMat4("view", view);
        myshader.setMat4("model", model);
        myshader.setVec4("ourColor", 0.0f, 0.0f, 0.0f, 1.0f);

        background.drawObject();

        glBindVertexArray(0);

        // draw platforms-----------------------------------------------------------------
        model = glm::mat4(1.0f);

        myshader.setMat4("view", view);
        myshader.setMat4("model", model);
        myshader.setVec4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);

        platforms.drawObject();

        glBindVertexArray(0);

        // -------------------------------------------------------------------------------

        if(over || win)
        {
            std::string dist = "Distance : ";
            dist += std::to_string((int)(distance + ldist * (Level - 1)));
            dist += "m";

            std::string level = "Level : ";
            level += std::to_string(Level);

            std::string coins = "Coins : ";
            coins += std::to_string(coinswon);

            RenderText(textshader, dist, -width, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
            RenderText(textshader, level, -75.0f, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
            RenderText(textshader, coins, width - 150, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
            if(over)
            {
                // game over
                RenderText(textshader, "Game", -300.0f, 50.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
                RenderText(textshader, "Over", -225.0f, -150.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
            }
            if(win)
            {
                // you win
                RenderText(textshader, "You", -200.0f, 50.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), VAO, VBO);
                RenderText(textshader, "Win", -175.0f, -150.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), VAO, VBO);
            }
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // draw player-------------------------------------------------------------------------------
        playershader.use();
        playershader.setMat4("projection", projection);
        
        model = glm::mat4(1.0f);

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

        playershader.setMat4("view", view);
        playershader.setMat4("model", model);
        playershader.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);

        player.drawObject();

        glBindVertexArray(0);

        // draw coins---------------------------------------------------------------------
        myshader.use();
        myshader.setMat4("projection", projection);
        myshader.setMat4("view", view);

        model = glm::mat4(1.0f);

        if(coinCounter == 0 && distance < ldist - 10)
        {
            coinCounter = cc;
            coin paisa;
            coinQueue.push_back(paisa);
        }
        else
        {
            coinCounter -= Level;
        }

        // if(coinCounter == 0)
        // {
        //     coinCounter++;
        //     coin paisa;
        //     coinQueue.push_back(paisa);
        // }


        for(int i = 0; i < coinQueue.size(); i++)
        {
            if(coinQueue[i].pos.x < -750.0)
            {
                coinQueue.pop_front();
                i--;
                continue;
            }

            model = glm::mat4(1.0f);

            coinQueue[i].pos.x -= coinSpeed * deltaTime;
            model = glm::translate(model, coinQueue[i].pos);
            if(coinQueue[i].show)
            {
                if(coinQueue[i].checkCol(playerPos, px, py))
                {
                    coinQueue[i].show = false;
                    coinswon++;
                }
                coinRotate = coinRotate + coinAngSpeed;

                if(coinRotate >= 180)
                {
                    coinRotate -= 180;
                }

                model = glm::rotate(model, glm::radians(coinRotate), glm::vec3(0.0f, 1.0f, 0.0f));
                myshader.setMat4("model", model);

                myshader.setVec4("ourColor", 1.0f, 1.0f, 0.0f, 1.0f);
                coinFaces.drawObject();
                myshader.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);
                coinSides.drawObject();

                glBindVertexArray(0);
            }
        }

        // draw obstacles-----------------------------------------------------------------
        obsshader.use();
        obsshader.setMat4("projection", projection);
        obsshader.setMat4("view", view);

        if(spawnCounter <= 0 && distance < ldist - 10)
        {
            spawnCounter = obsCount;
            obstacle zapper(Level);
            obsQueue.push_back(zapper);
        }
        else
        {
            spawnCounter--;
        }

        // if(spawnCounter == 0)
        // {
        //     spawnCounter++;
        //     obstacle zapper(Level);
        //     obsQueue.push_back(zapper);
        // }

        for(int i = 0; i<obsQueue.size(); i++)
        {
            if(obsQueue[i].obsPos.x < -750.0)
            {
                obsQueue.pop_front();
                i--;
                continue;
            }

            
            model = glm::mat4(1.0f);

            obsQueue[i].obsPos.x -= obsSpeed * deltaTime;
            if(obsQueue[i].obsOsc)
            {
                obsQueue[i].obsPos.y = glm::sin(glm::radians(obsQueue[i].obsOscAngle++)) * obsOscPos;
            }

            model = glm::translate(model, obsQueue[i].obsPos);
            obsQueue[i].obsRotate = obsQueue[i].obsRotate + obsQueue[i].obsAngSpeed;
            if(obsQueue[i].obsRotate >= 180)
            {
                obsQueue[i].obsRotate -= 180;
            }
            model = glm::rotate(model, glm::radians(obsQueue[i].obsRotate), glm::vec3(0.0f, 0.0f, 1.0f));

            if(obsQueue[i].checkCol(playerPos, px, py, model))
            {
                over = true;
            }

            obsshader.setMat4("model", model);
            if(Level == 1)
            {
                obsshader.setVec4("ourColor", 1.0f, 1.0f, 0.0f, 1.0f);
            }
            if(Level == 2)
            {
                obsshader.setVec4("ourColor", 0.0f, 1.0f, 1.0f, 1.0f);
            }
            if(Level == 3)
            {
                obsshader.setVec4("ourColor", 0.5f, 0.0f, 1.0f, 1.0f);
            }

            obstacles.drawObject();

            glBindVertexArray(0);
        }

        // text-----------------------------------------------------------------------------
        textshader.use();
        textshader.setMat4("projection", projection);

        model = glm::mat4(1.0f);

        textshader.setMat4("view", view);
        textshader.setMat4("model", model);

        distance += obsSpeed * deltaTime / 100;
        std::string dist = "Distance : ";
        dist += std::to_string((int)distance);
        dist += "m/";
        dist += std::to_string(ldist);
        dist += "m";

        std::string level = "Level : ";
        level += std::to_string(Level);

        std::string coins = "Coins : ";
        coins += std::to_string(coinswon);

        RenderText(textshader, dist, -width, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
        RenderText(textshader, level, -75.0f, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);
        RenderText(textshader, coins, width - 150, 210.0f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), VAO, VBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
