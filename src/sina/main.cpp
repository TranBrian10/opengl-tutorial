#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define ERR_RTN -1

// settings
const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool compileShader(GLuint &shader, const char* fileName, GLenum shaderType, std::string &bufferStr);
bool linkProgram(GLuint &program, const GLuint vertexShader, const GLuint fragmentShader);

std::string VertexBufferStr;
std::string FragmentBufferStr;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Apply the window to the current working thread
    
    // Initialize GLAD before calling any OpenGL funcitons.
    // This is done because the functions are OS-specific
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Initial viewport mapping: taking from (-1, 1) to (0, 800) and (0, 600)
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    // Register callback for resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    bool success;
    success = compileShader(vertexShader ,"../../src/sina/GLSL/vertex.glsl", GL_VERTEX_SHADER, VertexBufferStr);
    success &= compileShader(fragmentShader, "../../src/sina/GLSL/fragment.glsl", GL_FRAGMENT_SHADER, FragmentBufferStr);
    
    
    // Shader program
    GLuint shaderProgram;
    success &= linkProgram(shaderProgram, vertexShader, fragmentShader);
    if (!success)
        return ERR_RTN;
    
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    GLuint indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    // Generate both the vertex buffer array and the Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind Vertex Array Object
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Either static, dynamic or stream draw. Static: less likely to change.
    // Dynamic: likely to change. Stream: will change on every frame.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // We are using EBOs(Element Buffer Object) to use less memory while defining geometries.
    // We would otherwise end up duplicating vertices that are on top of one another.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex
    // buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens.
    // Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly
    // necessary.
    glBindVertexArray(0);
    
    // To show out shape in WireFrame mode.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Rendering loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window); // Check if window needs to be closed
        
        // Actual rendering code
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // State-setting function
        glClear(GL_COLOR_BUFFER_BIT); // State-using function: uses the current state(set before)
        
        // ----------------- // ----------------- //
        // What we like to draw goes here:
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3); Used for drawing trangles.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // ----------------- // ----------------- //
        
        glfwSwapBuffers(window); // Related to the screen double buffer. Need to swap the front with the back buffer
        glfwPollEvents(); // Check for any events
    }
    
    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool compileShader (GLuint &shader, const char* fileName,
                    GLenum shaderType, std::string &bufferStr)
{
    const char *shaderName = shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "VERTEX";
    // Parsing the shader file. Other options would have been making
    // a whole string of code...
    std::ifstream shaderFile(fileName);
    std::ostringstream buffer;
    buffer << shaderFile.rdbuf();
    bufferStr = buffer.str();
    // Warning: safe only until bufferStr is destroyed or modified
    const GLchar *shaderSource = bufferStr.c_str();
    // Create shader
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    
    // Check if compilation was successful.
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderName << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    
    std::cout << shaderName << " Shader successful.\n" << std::endl;
    return true;
}

bool linkProgram(GLuint &program, const GLuint vertexShader, const GLuint fragmentShader)
{
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(success == GL_FALSE)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
        return false;
    }

    std::cout << "PROGRAM linkage successful." << std::endl;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}
