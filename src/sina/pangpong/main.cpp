#include <iostream>
#include <map>
#include <string>
#include <glad/glad.h>
//#define GLEW_STATIC
//#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.hpp"

#define ERR_RTN -1

// settings
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void fillCharacterMap(FT_Face &face);
void RenderBox(Shader &s, GLFWwindow *window, GLint player);
void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

std::string VertexBufferStr;
std::string FragmentBufferStr;

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAOs[3];
GLuint VBOs[3];
GLuint EBO;

GLfloat ctr_y1 = 0.0f;
GLfloat ctr_y2 = 0.0f;
GLfloat off_x = 0.05f;
GLfloat off_y = off_x * 6;

using namespace std;

int main()
{
    // Initial setup for GLFW
    // This required me to add serveral frameworks to get the many errors I saw
    // IOKit, Cocoa and CoreVideo frameworks
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Initalize a window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Tutorial", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Apply the window to the current working thread
    
    // Initialize GLEW to setup the OpenGL Function pointers
    //glewExperimental = GL_TRUE;
    //glewInit();
    
    // Initialize GLAD before calling any OpenGL funcitons.
    // This is done because the functions are OS-specific
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Initial viewport mapping: taking from (-1, 1) to (0, 800) and (0, 600)
    glViewport(0, 0, WIDTH, HEIGHT);
    // Register callback for resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader vfShader("../../src/sina/GLSL/vertex.glsl", "../../src/sina/GLSL/fragment.glsl");
    Shader object_vfShader("../../src/sina/GLSL/vertex_object.glsl", "../../src/sina/GLSL/fragment_object.glsl");
    // Set up the projection as orthographic. (text doesn't need perspective)
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    vfShader.use();
    glUniformMatrix4fv(glGetUniformLocation(vfShader.programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Font creation
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) // Intialize
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    
    FT_Face face;
    if (FT_New_Face(ft, "../../src/sina/fonts/open-sans/OpenSans-Regular.ttf", 0, &face)) // Load the font
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    
    // Sets the font's width and height parameters.
    // Setting the width to 0 lets the face dynamically calculate the width based on the given height.
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    
    fillCharacterMap(face);
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    ///////////////
    unsigned int indices[] = {  // note that we start from 0!
        3, 1, 0,   // first triangle
        3, 2, 1    // second triangle
    };
    
    // Generate both the vertex buffer array and the Vertex Array Object
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);
    // bind Vertex Array Object
    glBindVertexArray(VAOs[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    // Either static, dynamic or stream draw. Static: less likely to change.
    // Dynamic: likely to change. Stream: will change on every frame.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex
    // buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens.
    // Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly
    // necessary.
    glBindVertexArray(0);
    
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, NULL, GL_STATIC_DRAW);
    // We are using EBOs(Element Buffer Object) to use less memory while defining geometries.
    // We would otherwise end up duplicating vertices that are on top of one another.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, NULL, GL_STATIC_DRAW);
    // We are using EBOs(Element Buffer Object) to use less memory while defining geometries.
    // We would otherwise end up duplicating vertices that are on top of one another.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // To show out shape in WireFrame mode.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Rendering/Game loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window); // Check if window needs to be closed
        
        // Actual rendering code
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // State-setting function
        glClear(GL_COLOR_BUFFER_BIT); // State-using function: uses the current state(set before)
        
        // ----------------- // ----------------- //
        // What we like to draw goes here:
        RenderBox(object_vfShader, window, 1);
        RenderBox(object_vfShader, window, 2);
        RenderText(vfShader, "OpenGL Tutorial", 8.0f, 570.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f));
        RenderText(vfShader, "PangPong", 8.0f, 550.0f, 0.25f, glm::vec3(1.0, 1.0f, 1.0f));
        RenderText(vfShader, "0 : 0", 300.0f, 520.0f, 2.0f, glm::vec3(1.0, 1.0f, 1.0f));
        // ----------------- // ----------------- //
        
        glfwSwapBuffers(window); // Related to the screen double buffer. Need to swap the front with the back buffer
        glfwPollEvents(); // Check for any events
    }
    
    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(3, VBOs);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate(); // Clean GLFW properly
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Adjust the size of the window using this callback
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // Check if the ESCAPE key was pressed and set up condition to close the window passed as input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && ctr_y1 + off_y < 1.0f)
        ctr_y1 += 0.02f;
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && ctr_y1 - off_y > -1.0f)
        ctr_y1 -= 0.02f;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && ctr_y2 + off_y < 1.0f)
        ctr_y2 += 0.02f;
    else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && ctr_y2 - off_y > -1.0f)
        ctr_y2 -= 0.02f;
}

void fillCharacterMap(FT_Face &face)
{
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        
        // Generate texture
        GLuint texture;
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
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
}

void RenderBox(Shader &s, GLFWwindow *window, GLint player)
{
    GLfloat ctr_x = player == 1 ? -0.8f : 0.8f;
    GLfloat ctr_y = player == 1 ? ctr_y1 : ctr_y2;
    GLfloat vertices[] = {
        ctr_x + off_x, ctr_y + off_y, 0.0f,  // top right
        ctr_x + off_x, ctr_y - off_y, 0.0f,  // bottom right
        ctr_x - off_x, ctr_y - off_y, 0.0f,  // bottom left
        ctr_x - off_x, ctr_y + off_y,  0.0f  // top left
    };
    
    s.use();
    glBindVertexArray(VAOs[player]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[player]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    s.use();
    glUniform3f(glGetUniformLocation(s.programId, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAOs[0]);
    
    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
