#ifndef TEXT_H
#define TEXT_H

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

struct Character{
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    long int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

unsigned int VAO, VBO;

void charInit(FT_Face face)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(char c = 0; c<127; c++)
    {
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
}

void textInit()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color, unsigned int VAO, unsigned int VBO)
{
    shader.use();
    shader.setVec4("ourColor", glm::vec4(color, 1.0f));
    shader.setInt("text", 0);
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
            xpos, ypos + h, 10.0f, 0.0f, 0.0f,
            xpos, ypos, 10.0f, 0.0f, 1.0f,
            xpos + w, ypos, 10.0f, 1.0f, 1.0f,

            xpos, ypos + h, 10.0f, 0.0f, 0.0f,
            xpos + w, ypos, 10.0f, 1.0f, 1.0f,
            xpos + w, ypos + h, 10.0f, 1.0f, 0.0f,
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

#endif