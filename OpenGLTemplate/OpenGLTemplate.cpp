#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//Screen Dimensions Variables
const unsigned int screen_width = 1024;
const unsigned int screen_height = 768;
//Number of Vertices
const GLuint NumVertices = 6;

void processInput(GLFWwindow* window);

int main() {

   // Initialize GLFW and check if it works
   // ------------------------------
   if (!glfwInit()) {
      fprintf(stderr, "Failed to initiate GLFW!\n");
      return -1;
   }
   //Set some Hints for the next glfwCreateWindow call.
   glfwWindowHint(GLFW_SAMPLES, 4); //Antializing set to x4
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Set the OpenGL version to 4.x
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4); //Set the OpenGL version to 4.x
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Set compatibility with newer 
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Set OpenGL core profile

   //Apple compatibility
#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   // Open a window and create its OpenGL context
   GLFWwindow* window;
   window = glfwCreateWindow(screen_width, screen_height, "SimpleWindowwithColor", NULL, NULL);
   if (window == NULL) {
      fprintf(stderr, "Failed to open GLFW window\n");
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window); // Set the windows to render

   // Load all OpenGL function pointers and check if GLAD works…
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   //This loop renders the window we created above
   while (!glfwWindowShouldClose(window))
   {
      processInput(window);

      static const float red[] = { 1.0f, 0.0f, 0.0f, 0.0f };
      glClearBufferfv(GL_COLOR, 0, red);

      //Swap buffers
      glfwSwapBuffers(window);
      //Poll for Input/Output Events such a key pressed, mouse clicked etc...
      glfwPollEvents();
   }
   //Terminate, clearing all previously allocated GLFW resources.
   glfwTerminate();


   return 0;
}

//If a relevant key (e.g. Escape) is pressed then execute the code in the body. Herein close the window.
void processInput(GLFWwindow* window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}
