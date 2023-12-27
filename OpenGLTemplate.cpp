#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaderinit.h"
#include "stb_image.h"
#include <iostream>
#include <vector>

void fillBrickX_and_YPos();
void applyTexture(const char* imagePath, unsigned int& texture);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void init(void);
void drawSphere(Shader& ourShader);
void checkSphereCollision(GLFWwindow* window);
void drawPlatform(Shader& ourShader);
void checkPlatformCollision();
void drawRows(Shader& ourShader);
//window dimensions
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

unsigned int VAO1, VAO2, VAO3, VAO4, VAO5, VAOS, VAOP;
unsigned int VBO1, VBO2, VBO3, VBO4, VBO5, VBOS, VBOP;

bool moveSphere = false;
float sphereYPos = -3.3f;
float sphereXPos = 0.0f;
float sphereXSpeed = 0.004f;
float sphereYSpeed = 0.004f;

std::vector<float> brickXPos;
std::vector<float> brickYPos;
bool brickBroken[70] = { false };

bool movePlatformToLeft = false;
bool movePlatformToRight = false;
float platformXPos = 0.0f;
float platformYPos = -3.5f;

unsigned int platformTexture;
unsigned int sphereTexture;
unsigned int brick1Texture;
unsigned int brick2Texture;
unsigned int brick3Texture;


int main(void)
{
   // glfw: initialize and configure
   // ------------------------------
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
   // glfw window creation
   // --------------------
   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Arkanoid", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   // glad: load all OpenGL function pointers
   // ---------------------------------------
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   // configure global opengl state
   // -----------------------------
   glEnable(GL_DEPTH_TEST);

   // build and compile our shader zprogram
   // ------------------------------------

   Shader ourShader("shader.vs", "shader.fs");

   
   
   init();

   
   
   applyTexture("C:/Users/denni/Documents/GAM531FinalPRJDB/assets/purple.jpg", platformTexture);
   applyTexture("C:/Users/denni/Documents/GAM531FinalPRJDB/assets/red.jpg", sphereTexture);
   applyTexture("C:/Users/denni/Documents/GAM531FinalPRJDB/assets/lightblue.jpg", brick1Texture);
   applyTexture("C:/Users/denni/Documents/GAM531FinalPRJDB/assets/yellow.jpg", brick2Texture);
   applyTexture("C:/Users/denni/Documents/GAM531FinalPRJDB/assets/pink.jpg", brick3Texture);

   ourShader.use();
   ourShader.setInt("platformTexture", 0);
   ourShader.setInt("brick2Texture", 1);
   ourShader.setInt("brick3Texture", 2);

   fillBrickX_and_YPos();

   while (!glfwWindowShouldClose(window))
   {
      processInput(window);

      checkPlatformCollision();
      checkSphereCollision(window);

      if (moveSphere) {
         sphereXPos += sphereXSpeed;
         sphereYPos += sphereYSpeed;
      }
      
      //speed for platform 
      if (movePlatformToLeft)
         platformXPos -= 0.004f;
      else if (movePlatformToRight)
         platformXPos += 0.004f;

     
      glClearColor(0.0f, 0.5f, 0.5f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      ourShader.use();

      //static 3/4 Perspective Projection
      glm::mat4 view = glm::mat4(1.0f);
      glm::mat4 projection = glm::mat4(1.0f);
      projection = glm::perspective(glm::radians(75.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

      ourShader.setMat4("projection", projection);
      ourShader.setMat4("view", view);

      
      
      
      // Get the game set up
      drawRows(ourShader);
      drawPlatform(ourShader);
      drawSphere(ourShader);

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      // -------------------------------------------------------------------------------
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;

}




void init(void)
{
   float vertices1[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   glGenVertexArrays(1, &VAO1);
   glGenBuffers(1, &VBO1);
   glBindVertexArray(VAO1);
   glBindBuffer(GL_ARRAY_BUFFER, VBO1);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   
   
   
   // 2nd row

   float vertices2[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   glGenVertexArrays(1, &VAO2);
   glGenBuffers(1, &VBO2);
   glBindVertexArray(VAO2);
   glBindBuffer(GL_ARRAY_BUFFER, VBO2);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   
   
   
   
   // 3rd row

   float vertices3[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   glGenVertexArrays(1, &VAO3);
   glGenBuffers(1, &VBO3);

   glBindVertexArray(VAO3);

   glBindBuffer(GL_ARRAY_BUFFER, VBO3);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   // 4th row

   float vertices4[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   glGenVertexArrays(1, &VAO4);
   glGenBuffers(1, &VBO4);

   glBindVertexArray(VAO4);

   glBindBuffer(GL_ARRAY_BUFFER, VBO4);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices4), vertices4, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   // 5th row

   float vertices5[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   glGenVertexArrays(1, &VAO5);
   glGenBuffers(1, &VBO5);

   glBindVertexArray(VAO5);

   glBindBuffer(GL_ARRAY_BUFFER, VBO5);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices5), vertices5, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);


   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);


   // Platform

   float verticesP[] = {
        0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.1f, 0.4f, 0.7f, 0.0f, 1.0f
   };
   glGenVertexArrays(1, &VAOP);
   glGenBuffers(1, &VBOP);

   glBindVertexArray(VAOP);

   glBindBuffer(GL_ARRAY_BUFFER, VBOP);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verticesP), verticesP, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   // Sphere/Cube
   float verticesS[] = {
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };
   glGenVertexArrays(1, &VAOS);
   glGenBuffers(1, &VBOS);

   glBindVertexArray(VAOS);

   glBindBuffer(GL_ARRAY_BUFFER, VBOS);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verticesS), verticesS, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);


   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawRows(Shader& ourShader)
{

   int brickNumber = 0;

   // 1st row
   //bricks are drawn using 3D cubes
   glBindVertexArray(VAO1);
   glActiveTexture(brick1Texture);
   //texture being used in row
   glBindTexture(GL_TEXTURE_2D, brick1Texture);
   for (unsigned int i = 0; i < 14; i++)
   {
      brickNumber += 1;
      if (brickBroken[brickNumber])
         continue;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4.6 + i * 0.8f, 3.4f, -5.0f));
      model = glm::scale(model, glm::vec3(0.69f, 0.2f, 0.2f));
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }


   glBindVertexArray(VAO2);
   glActiveTexture(brick2Texture);//Apply texture
   glBindTexture(GL_TEXTURE_2D, brick2Texture);// Bind the brick2Texture
   for (unsigned int i = 0; i < 14; i++)
   {
      brickNumber += 1;
      if (brickBroken[brickNumber])
         continue;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4.6 + i * 0.8f, 3.4f - 0.25f * 1.0f, -5.0f));
      model = glm::scale(model, glm::vec3(0.69f, 0.2f, 0.2f));
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }

   glBindVertexArray(VAO3);
   glActiveTexture(brick3Texture);// Activate the texture for row 3
   glBindTexture(GL_TEXTURE_2D, brick3Texture);// Bind the brick3Texture
   for (unsigned int i = 0; i < 14; i++)
   {
      brickNumber += 1;
      if (brickBroken[brickNumber])
         continue;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4.6 + i * 0.8f, 3.4f - 0.25f * 2.0f, -5.0f));
      model = glm::scale(model, glm::vec3(0.69f, 0.2f, 0.2f));
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }
   //Textures are applied to the sphere and bricks in the applyTexture function
   glBindVertexArray(VAO4);
   glActiveTexture(brick1Texture);//Apply texture
   glBindTexture(GL_TEXTURE_2D, brick1Texture);//Bind texture
   for (unsigned int i = 0; i < 14; i++)
   {
      brickNumber += 1;
      if (brickBroken[brickNumber])
         continue;
      //View, Model, Projection Model Implemented
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4.6 + i * 0.8f, 3.4f - 0.25f * 3.0f, -5.0f));
      model = glm::scale(model, glm::vec3(0.69f, 0.2f, 0.2f));
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }

   glBindVertexArray(VAO5);
   glActiveTexture(brick2Texture);
   glBindTexture(GL_TEXTURE_2D, brick2Texture);
   for (unsigned int i = 0; i < 14; i++)
   {
      brickNumber += 1;
      if (brickBroken[brickNumber])
         continue;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-4.6 + i * 0.8f, 3.4f - 0.25f * 4.0f, -5.0f));
      model = glm::scale(model, glm::vec3(0.69f, 0.2f, 0.2f));
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }

   glBindVertexArray(0);
}
void drawSphere(Shader& ourShader)
{
   //textures are applied to the sphere
   glBindVertexArray(VAOS);
   glActiveTexture(sphereTexture);
   //sphere texture being used
   glBindTexture(GL_TEXTURE_2D, sphereTexture);
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, glm::vec3(sphereXPos, sphereYPos, -5.0f));
   model = scale(model, glm::vec3(0.5f, 0.5f, 0.25f));
   ourShader.setMat4("model", model);
   glDrawArrays(GL_TRIANGLES, 0, 36);

   glBindVertexArray(0);
}



void drawPlatform(Shader& ourShader)
{
   //platform is also drawn using a 3D shape
   glBindVertexArray(VAOP);
   //platform texture and color
   glActiveTexture(platformTexture);
   glBindTexture(GL_TEXTURE_2D, platformTexture);
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, glm::vec3(platformXPos, platformYPos, -5.0f));
   model = scale(model, glm::vec3(2.7f, 0.2f, 0.2f));
   ourShader.setMat4("model", model);
   glDrawArrays(GL_TRIANGLES, 0, 36);

   glBindVertexArray(0);
}
void checkPlatformCollision()
{
   //left boundary check
   if (movePlatformToLeft) {
      if (platformXPos <= -4.4f) {
         movePlatformToLeft = false;
         movePlatformToRight = true;
      }
   }

   // right boundary check
   if (movePlatformToRight) {
      if (platformXPos >= 4.4f) {
         movePlatformToLeft = true;
         movePlatformToRight = false;
      }
   }
}
void checkSphereCollision(GLFWwindow* window)
{
   //collision Detection sphere
   // boundaries 
   // walls
   // left boundary check
   if (sphereXPos <= -4.8f)
      sphereXSpeed = 0.002f;

   // right boundary check
   if (sphereXPos >= 4.8f)
      sphereXSpeed = -0.002f;

   // collision with brick check
   if (sphereYPos >= brickYPos.back() - 0.2f) {
      int brickRow = 0;
      for (brickRow = 0; brickRow < brickYPos.size() - 1; ++brickRow) {
         if (sphereYPos >= brickYPos[brickRow] - 0.2f)
            break;
      }
      brickRow += 1;

      int brickNumber = 0;
      for (brickNumber = brickXPos.size() - 1; brickNumber >= 0; --brickNumber)
         if (sphereXPos > brickXPos[brickNumber] - 0.69f)
            break;
      brickNumber += 1;

      // erase the brick
      if (!brickBroken[((brickRow - 1) * 14) + (brickNumber - 1)]) {
         brickBroken[((brickRow - 1) * 14) + (brickNumber - 1)] = true;
         sphereYSpeed = -0.002f;
      }
   }

   // collision platform check
   //Game End Condition
   if (sphereYPos <= platformYPos + 0.2f) {
      if (sphereXPos >= platformXPos - 0.6f && sphereXPos <= platformXPos + 0.6f)
         sphereYSpeed = 0.002f;
      else
         glfwSetWindowShouldClose(window, true); // end the game
   }


}

void applyTexture(const char* imagePath, unsigned int& texture)
{
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   int width, height, nrChannels;
   unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
   if (data)
   {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
   }
   else
   {
      std::cout << "Failed to load texture" << std::endl;
   }
   stbi_image_free(data);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
   //escape key to exit game
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
   //space to move sphere
   else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      moveSphere = true;
   //checks if left
   else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      movePlatformToLeft = true;
      movePlatformToRight = false;
   }
   //check if right
   else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      movePlatformToLeft = false;
      movePlatformToRight = true;
   }
}

void fillBrickX_and_YPos()
{
   // Y Position filling
   for (int i = 0; i < 5; ++i)
      brickYPos.push_back(3.4f - 0.25f * (float)i);

   for (int i = 0; i < 14; ++i)
      brickXPos.push_back(-4.6f + (float)i * 0.71f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // make sure the viewport matches the new window dimensions; note that width and 
   // height will be significantly larger than specified on retina displays.
   glViewport(0, 0, width, height);
}