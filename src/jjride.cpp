// OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
float upacc = 15;
float downacc = -15;
float uplim = 160;
float downlim = -160;
float distance = 0;

// window
int windowWidth;
int windowHeight;

// background
float behindspeed = 20;
float behindpos = 0;
float frontspeed = 30;
float frontpos = 0;

// platform
float platSpeed = 200;
float platpos = 0;

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
    Shader bgshader("../src/vertex.shader", "../src/bgfrag.shader");
    Shader frontshader("../src/vertex.shader", "../src/frontfrag.shader");
    Shader platshader("../src/vertex.shader", "../src/platfrag.shader");

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
        400.0f, 250.0f, 0.0f, 1.0f, 1.0f,
        -400.0f, 250.0f, 0.0f, 0.0f, 1.0f,
        -400.0f, -250.0f, 0.0f, 0.0f, 0.0f,
        400.0f, -250.0f, 0.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> bgInd{
        0, 1, 2,
        0, 2, 3
    };

    Mesh background(bgVert, bgInd);

    std::vector<float> platformVert{
        25.0f, -200.0f, 0.0f, 0.286f, 0.99f,
        -25.0f, -200.0f, 0.0f, 0.143f, 0.99f,
        -25.0f, -250.0f, 0.0f, 0.143f, 0.86f,
        25.0f, -250.0f, 0.0f, 0.286f, 0.86f,
    };
    std::vector<unsigned int> platformInd{
        0, 1, 2,
        0, 2, 3,
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

    unsigned int bg1, bg2, bg3, plat;
    int iwidth, iheight, nocc;
    unsigned char *data;

    glGenTextures(1, &bg1);
    glBindTexture(GL_TEXTURE_2D, bg1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../resources/background_layer_1.png", &iwidth, &iheight, &nocc, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &bg2);
    glBindTexture(GL_TEXTURE_2D, bg2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../resources/background_layer_2.png", &iwidth, &iheight, &nocc, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    bgshader.use();
    bgshader.setInt("bg1", 0);
    bgshader.setInt("bg2", 1);

    glGenTextures(1, &bg3);
    glBindTexture(GL_TEXTURE_2D, bg3);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../resources/background_layer_3.png", &iwidth, &iheight, &nocc, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    frontshader.use();
    frontshader.setInt("bg3", 2);

    glGenTextures(1, &plat);
    glBindTexture(GL_TEXTURE_2D, plat);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../resources/MossyTileSet.png", &iwidth, &iheight, &nocc, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    platshader.use();
    platshader.setInt("plat", 3);

    while(!glfwWindowShouldClose(window))
    {
        float ratio = (float)windowHeight / windowWidth;
        float width = 400.0f;
        float height = width * ratio;
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bg1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bg2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, bg3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, plat);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        myshader.use();
        view = glm::lookAt(camPos, camPos + camFront, camUp);
        myshader.setMat4("projection", projection);

        // draw background----------------------------------------------------------------
        bgshader.use();
        bgshader.setMat4("projection", projection);

        if(behindpos < -800)
        {
            behindpos = 0;
        }
        behindpos -= behindspeed * deltaTime;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(behindpos, 0.0f, -50.0f));
        bgshader.setMat4("model", model);
        background.drawObject();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(behindpos + 800.0f, 0.0f, -50.0f));
        bgshader.setMat4("model", model);
        background.drawObject();

        glBindVertexArray(0);

        frontshader.use();
        frontshader.setMat4("projection", projection);

        if(frontpos < -800)
        {
            frontpos = 0;
        }
        frontpos -= frontspeed * deltaTime;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(frontpos, 0.0f, -40.0f));
        frontshader.setMat4("model", model);
        background.drawObject();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(frontpos + 800.0f, 0.0f, -40.0f));
        frontshader.setMat4("model", model);
        background.drawObject();

        glBindVertexArray(0);

        // draw platforms-----------------------------------------------------------------
        platshader.use();
        platshader.setMat4("projection", projection);

        platpos -= platSpeed * deltaTime;
        if(platpos < -800)
        {
            platpos = 0;
        }

        for(int i = 0; i < 32; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(platpos + 50 * i - 400, 0.0f, 0.0f));
            platshader.setMat4("model", model);

            platforms.drawObject();
        }

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

            RenderText(textshader, dist, -width, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);
            RenderText(textshader, level, -75.0f, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);
            RenderText(textshader, coins, width - 150, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);
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

        playershader.setMat4("model", model);
        playershader.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);

        player.drawObject();

        glBindVertexArray(0);

        // draw coins---------------------------------------------------------------------
        myshader.use();
        myshader.setMat4("projection", projection);

        model = glm::mat4(1.0f);

        if(coinCounter <= 0 && distance < ldist - 10)
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

        RenderText(textshader, dist, -width, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);
        RenderText(textshader, level, -75.0f, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);
        RenderText(textshader, coins, width - 150, 210.0f, 0.15f, glm::vec3(1.0f, 1.0f, 1.0f), VAO, VBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
