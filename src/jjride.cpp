// OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

// Custom Headers
#include <shader.h>
#include <mesh.h>
#include <obstacle.h>

// C++ Headers
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <map>

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

// platform
float platSpeed = 0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// characters
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    long int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color, unsigned int VAO, unsigned int VBO)
{
    shader.use();
    shader.setVec4("ourColor", glm::vec4(color, 1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for(int i = 0; i < text.length(); i++)
    {
        Character ch = Characters[text[i]];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        std::vector<float> vertices{
            xpos, ypos + h, 0.0f, 0.0f, 0.0f,
            xpos, ypos, 0.0f, 0.0f, 1.0f,
            xpos + w, ypos, 0.0f, 1.0f, 1.0f,

            xpos, ypos + h, 0.0f, 0.0f, 0.0f,
            xpos + w, ypos, 0.0f, 0.0f, 1.0f,
            xpos + w, ypos, 0.0f, 1.0f, 1.0f,
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices.front());

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../fonts/Arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(char c = 0; c<127; c++)
    {
        // std::cout << c;
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

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

    std::vector<float> platformVert{
        -450.0f, -200.0f, 0.0f, 1.0f, 1.0f,
        -450.0f, -250.0f, 0.0f, 0.0f, 1.0f,
        450.0f, -250.0f, 0.0f, 0.0f, 0.0f,
        450.0f, -200.0f, 0.0f, 1.0f, 0.0f,

        -450.0f, 200.0f, 0.0f, 1.0f, 1.0f,
        -450.0f, 250.0f, 0.0f, 0.0f, 1.0f,
        450.0f, 250.0f, 0.0f, 0.0f, 0.0f,
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

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(-400.0f, 400.0f, -250.0f, 250.0f, -1.0f, 1.0f);
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
        myshader.setVec4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);

        player.drawObject();

        //draw platforms
        view = glm::mat4(1.0f);
        model = glm::mat4(1.0f);

        view = glm::lookAt(camPos, camPos + camFront, camUp);

        myshader.setMat4("view", view);
        myshader.setMat4("model", model);
        myshader.setVec4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);

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
            if(obsQueue[i].obsPos.x < -750.0)
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
            myshader.setVec4("ourColor", 1.0f, 1.0f, 0.0f, 1.0f);

            obstacles.drawObject();

            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
