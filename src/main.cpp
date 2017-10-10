#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);
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
    glViewport(0, 0, 800, 600);
    
    // Register callback for resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Rendering loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window); // Check if window needs to be closed
        
        // Actual rendering code
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // State-setting function
        glClear(GL_COLOR_BUFFER_BIT); // State-using function: uses the current state(set before)
        
        glfwSwapBuffers(window); // Related to the screen double buffer. Need to swap the front with the back buffer
        glfwPollEvents(); // Check for any events
    }
    
    glfwTerminate(); // Clean GLFW properly
    return 0;
}
