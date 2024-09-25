#include <glad/glad.h>
#include "cylinder.h"
#include "cone.h"
#include "ShapeGenerator.h"
#include "ShapeData.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "filesystem.h"
#include "shader_m.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
int createTorus(double r, double c, int rSeg, int cSeg, float scaler, GLfloat** vertices, GLfloat** uv);
void setCoords(double r, double c, int rSeg, int cSeg, int i, int j, GLfloat* vertices, GLfloat* uv);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(-2.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float defaultCameraSpeed = 2.5f;
float scale = 100.0f;
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// sphere
GLuint sphereNumIndices;
GLuint sphereVertexArrayObjectID;
GLuint sphereIndexByteOffset;

GLuint sphereNumIndices2;
GLuint sphereVertexArrayObjectID2;
GLuint sphereIndexByteOffset2;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

// lighting
glm::vec3 lightPos(-2.0f, 2.0f, 2.0f);
glm::vec3 spotlight(0.0f, 0.0f, 0.0f);

int main()
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CS330 Project 2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices1[] = {
        //first cube			//Normals			 //Textures
         0.0f,  0.0f,  -0.5f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,
         0.3f,  0.0f,  -0.5f,   0.0f,  0.0f,  -1.0f,   1.0f, 0.0f,
         0.3f,  0.02f, -0.5f,   0.0f,  0.0f,  -1.0f,   1.0f, 1.0f,
         0.3f,  0.02f, -0.5f,   0.0f,  0.0f,  -1.0f,   1.0f, 1.0f,
         0.0f,  0.02f, -0.5f,   0.0f,  0.0f,  -1.0f,   0.0f, 1.0f,
         0.0f,  0.0f,  -0.5f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,

         0.0f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         0.3f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         0.3f,  0.02f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         0.3f,  0.02f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         0.0f,  0.02f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
         0.0f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

         0.0f,  0.02f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.0f,  0.02f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.0f,  0.0f,  -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.0f,  0.0f,  -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.0f,  0.0f,   0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.0f,  0.02f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

         0.3f,  0.02f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.3f,  0.02f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.3f,  0.0f,  -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.3f,  0.0f,  -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.3f,  0.0f,   0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.3f,  0.02f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

         0.0f,  0.0f,  -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.3f,  0.0f,  -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.3f,  0.0f,   0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.3f,  0.0f,   0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.0f,  0.0f,   0.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
         0.0f,  0.0f,  -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

         0.0f,  0.02f,  -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         0.3f,  0.02f,  -0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
         0.3f,  0.02f,   0.0f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.3f,  0.02f,   0.0f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.0f,  0.02f,   0.0f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         0.0f,  0.02f,  -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

         //second cube
         0.05f,  0.0f,  -0.55f,  0.0f,  0.0f,  -1.0f,  0.0f, 0.0f,
         0.25f,  0.0f,  -0.55f,  0.0f,  0.0f,  -1.0f,  1.0f, 0.0f,
         0.25f,  0.02f, -0.55f,  0.0f,  0.0f,  -1.0f,  1.0f, 1.0f,
         0.25f,  0.02f, -0.55f,  0.0f,  0.0f,  -1.0f,  1.0f, 1.0f,
         0.05f,  0.02f, -0.55f,  0.0f,  0.0f,  -1.0f,  0.0f, 1.0f,
         0.05f,  0.0f,  -0.55f,  0.0f,  0.0f,  -1.0f,  0.0f, 0.0f,

         0.05f,  0.0f,   0.05f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.25f,  0.0f,   0.05f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.25f,  0.02f,  0.05f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.25f,  0.02f,  0.05f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.05f,  0.02f,  0.05f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
         0.05f,  0.0f,   0.05f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

         0.05f,  0.02f,  0.05f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.05f,  0.02f, -0.55f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.05f,  0.0f,  -0.55f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.05f,  0.0f,  -0.55f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.05f,  0.0f,   0.05f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.05f,  0.02f,  0.05f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.25f,  0.02f,  0.05f,  1.0f,   0.0f,  0.0f,  1.0f, 0.0f,
         0.25f,  0.02f, -0.55f,  1.0f,   0.0f,  0.0f,  1.0f, 1.0f,
         0.25f,  0.0f,  -0.55f,  1.0f,   0.0f,  0.0f,  0.0f, 1.0f,
         0.25f,  0.0f,  -0.55f,  1.0f,   0.0f,  0.0f,  0.0f, 1.0f,
         0.25f,  0.0f,   0.05f,  1.0f,   0.0f,  0.0f,  0.0f, 0.0f,
         0.25f,  0.02f,  0.05f,  1.0f,   0.0f,  0.0f,  1.0f, 0.0f,

         0.05f,  0.0f,  -0.55f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.25f,  0.0f,  -0.55f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.25f,  0.0f,   0.05f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.25f,  0.0f,   0.05f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.05f,  0.0f,   0.05f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
         0.05f,  0.0f,  -0.55f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

         0.05f,  0.02f, -0.55f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         0.25f,  0.02f, -0.55f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
         0.25f,  0.02f,  0.05f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.25f,  0.02f,  0.05f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.05f,  0.02f,  0.05f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         0.05f,  0.02f, -0.55f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f
    };
    //indeces for plane used as phone screen
    float vertices2[] = {
        //first cube			 //Normals			//Textures
        0.02f,  0.022f, -0.53f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        0.28f,  0.022f, -0.53f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        0.28f,  0.022f,  0.03f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.28f,  0.022f,  0.03f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        0.02f,  0.022f,  0.03f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        0.02f,  0.022f, -0.53f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    // indeces for plane used as table and texture for table
    float vertices3[] = {
        // positions          //Normals          // texture coords
        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,	// front left
         0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,	// front right
         0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,	// back right
         0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,	// back right
        -0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,	// back left 
        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f	    // front left

    };

    float lightCube[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    // vertices for square remote portion of remote control including indeces for texture
    float vertices4[] = {
        0.0f,  0.0f,   0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front bottom left
        0.2f,  0.0f,   0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front bottom right
        0.0f,  0.04f,  0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front top left
        0.2f,  0.04f,  0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front top right
        0.2f,  0.0f,   0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front bottom right
        0.0f,  0.04f,  0.0f,   0.0f,  0.0f,  -1.0f,   0.0f, 0.0f,	// front top left

        0.0f,  0.04f,  1.0f,   0.0f,  1.0f,  0.0f,   0.35f, 1.0f,	// back top left
        0.0f,  0.04f,  0.0f,   0.0f,  1.0f,  0.0f,   0.35f, 0.0f,	// front top left
        0.2f,  0.04f,  0.0f,   0.0f,  1.0f,  0.0f,   0.65f, 0.0f,	// front top right
        0.2f,  0.04f,  1.0f,   0.0f,  1.0f,  0.0f,   0.65f, 1.0f,	// back top right
        0.2f,  0.04f,  0.0f,   0.0f,  1.0f,  0.0f,   0.65f, 0.0f,	// front top right
        0.0f,  0.04f,  1.0f,   0.0f,  1.0f,  0.0f,   0.35f, 1.0f,	// back top left

        0.0f,  0.0f,  1.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// back bottom left
        0.2f,  0.0f,  1.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// back bottom right
        0.2f,  0.0f,  0.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// front bottom right
        0.0f,  0.0f,  1.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// back bottom left
        0.0f,  0.0f,  0.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// front bottom left
        0.2f,  0.0f,  0.0f,    0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,	// front bottom right

        0.0f,  0.04f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back top left
        0.2f,  0.04f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back top right
        0.0f,  0.0f,   1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back bottom left
        0.2f,  0.0f,   1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back bottom right
        0.2f,  0.04f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back top right
        0.0f,  0.0f,   1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back bottom left

        0.2f,  0.04f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front top right
        0.2f,  0.04f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back top right
        0.2f,  0.0f,   0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front bottom right
        0.2f,  0.0f,   1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back bottom right
        0.2f,  0.04f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back top right
        0.2f,  0.0f,   0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front bottom right

        0.0f,  0.04f,  0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front top left
        0.0f,  0.04f,  1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back top left
        0.0f,  0.0f,   0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front bottom left
        0.0f,  0.0f,   1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back bottom left
        0.0f,  0.04f,  1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back top left
        0.0f,  0.0f,   0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f	// front bottom left
    };

    float coaster[] = {
        //partial cube			//Normals			 //Textures
        0.0f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// front bottom left
        0.5f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,	// front bottom right
        0.0f,  0.05f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.05f,	// front top left
        0.5f,  0.05f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.05f,	// front top right
        0.5f,  0.0f,   0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,	// front bottom right
        0.0f,  0.05f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.05f,	// front top left

        0.0f,  0.0f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,	// back bottom left
        0.5f,  0.0f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f,	// back bottom right
        0.5f,  0.0f,  0.0f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,	// front bottom right
        0.0f,  0.0f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,	// back bottom left
        0.0f,  0.0f,  0.0f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f,	// front bottom left
        0.5f,  0.0f,  0.0f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,	// front bottom right

        0.0f,  0.05f, 0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.05f,	// back top left
        0.5f,  0.05f, 0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.05f,	// back top right
        0.0f,  0.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back bottom left
        0.5f,  0.0f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,	// back bottom right
        0.5f,  0.05f, 0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.05f,	// back top right
        0.0f,  0.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,	// back bottom left

        0.5f,  0.05f, 0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.05f,	// front top right
        0.5f,  0.05f, 0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.05f,	// back top right
        0.5f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front bottom right
        0.5f,  0.0f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,	// back bottom right
        0.5f,  0.05f, 0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.05f,	// back top right
        0.5f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// front bottom right

        0.0f,  0.05f, 0.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.05f,	// front top left
        0.0f,  0.05f, 0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.05f,	// back top left
        0.0f,  0.0f,  0.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,	// front bottom left
        0.0f,  0.0f,  0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,	// back bottom left
        0.0f,  0.05f, 0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.05f,	// back top left
        0.0f,  0.0f,  0.0f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f	// front bottom left
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        lightPos,
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, VAO;
    unsigned int VBO2, VAO2;
    unsigned int VBO3, VAO3;
    unsigned int VBO4, VAO4;
    unsigned int coasterVBO, coasterVAO;
    unsigned int coneVBO, coneVAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Create and Load vertices into the buffer
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindVertexArray(VAO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create and Load vertices into the buffer
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);

    glBindVertexArray(VAO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create and Load vertices into the buffer
    glGenVertexArrays(1, &VAO4);
    glGenBuffers(1, &VBO4);

    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices4), vertices4, GL_STATIC_DRAW);

    glBindVertexArray(VAO4);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &coasterVAO);
    glGenBuffers(1, &coasterVBO);
    glBindVertexArray(coasterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, coasterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coaster), coaster, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCube), lightCube, GL_STATIC_DRAW);

    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLfloat* g_vertex_buffer_data;
    GLfloat* g_uv_buffer_data;
    int halfTorus = createTorus(0.01, 0.1, 100, 100, 0.48f, &g_vertex_buffer_data,
        &g_uv_buffer_data);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, halfTorus * 3 * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, halfTorus * 2 * sizeof(GLfloat), g_uv_buffer_data, GL_STATIC_DRAW);

    GLfloat* g_vertex_buffer_data2;
    GLfloat* g_uv_buffer_data2;
    int torus = createTorus(0.018, 0.18, 100, 100, 1.0f, &g_vertex_buffer_data2,
        &g_uv_buffer_data2);

    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, torus * 3 * sizeof(GLfloat), g_vertex_buffer_data2, GL_STATIC_DRAW);

    GLuint uvbuffer2;
    glGenBuffers(1, &uvbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
    glBufferData(GL_ARRAY_BUFFER, torus * 2 * sizeof(GLfloat), g_uv_buffer_data2, GL_STATIC_DRAW);



    GLfloat* g_vertex_buffer_data3;
    GLfloat* g_uv_buffer_data3;
    int partTorus = createTorus(0.018, 0.18, 100, 100, 0.75f, &g_vertex_buffer_data3,
        &g_uv_buffer_data3);

    GLuint vertexbuffer3;
    glGenBuffers(1, &vertexbuffer3);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
    glBufferData(GL_ARRAY_BUFFER, partTorus * 3 * sizeof(GLfloat), g_vertex_buffer_data3, GL_STATIC_DRAW);

    GLuint uvbuffer3;
    glGenBuffers(1, &uvbuffer3);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer3);
    glBufferData(GL_ARRAY_BUFFER, partTorus * 2 * sizeof(GLfloat), g_uv_buffer_data3, GL_STATIC_DRAW);

    GLfloat* g_vertex_buffer_data4;
    GLfloat* g_uv_buffer_data4;
    int torus2 = createTorus(0.03, 0.07, 100, 100, 1.0f, &g_vertex_buffer_data4,
        &g_uv_buffer_data4);

    GLuint vertexbuffer4;
    glGenBuffers(1, &vertexbuffer4);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer4);
    glBufferData(GL_ARRAY_BUFFER, torus2 * 3 * sizeof(GLfloat), g_vertex_buffer_data4, GL_STATIC_DRAW);

    GLuint uvbuffer4;
    glGenBuffers(1, &uvbuffer4);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer4);
    glBufferData(GL_ARRAY_BUFFER, torus2 * 2 * sizeof(GLfloat), g_uv_buffer_data4, GL_STATIC_DRAW);

    // creates sphere object
    ShapeData sphere = ShapeGenerator::makeSphere();
    GLsizeiptr currentOffset = 0;
    unsigned int sphereVBO{}, sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);
    currentOffset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
    currentOffset += sphere.vertexBufferSize();
    sphereIndexByteOffset = currentOffset;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
    sphereNumIndices = sphere.numIndices;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO);

    ShapeData sphere2 = ShapeGenerator::makeSphere();
    unsigned int sphereVBO2{}, sphereVAO2;
    glGenVertexArrays(1, &sphereVAO2);
    glGenBuffers(1, &sphereVBO2);
    glBindVertexArray(sphereVAO2);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO2);
    glBufferData(GL_ARRAY_BUFFER, sphere2.vertexBufferSize() + sphere2.indexBufferSize(), 0, GL_STATIC_DRAW);
    currentOffset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere2.vertexBufferSize(), sphere2.vertices);
    currentOffset += sphere2.vertexBufferSize();
    sphereIndexByteOffset2 = currentOffset;
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere2.indexBufferSize(), sphere2.indices);
    sphereNumIndices2 = sphere2.numIndices;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO2);

    glGenVertexArrays(1, &coneVAO);
    glGenBuffers(1, &coneVBO);
    glBindVertexArray(coneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, coneVBO);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("images/container2.png");
    unsigned int specularMap = loadTexture("images/container2_BW.png");
    unsigned int remoteDiffuseMap = loadTexture("images/remote.png");
    unsigned int remoteSpecularMap = loadTexture("images/remote_BW.png");
    unsigned int screenDiffuseMap = loadTexture("images/container3.png");
    unsigned int screenSpecularMap = loadTexture("images/container3_BW.png");
    unsigned int tableDiffuseMap = loadTexture("images/container.png");
    unsigned int tableSpecularMap = loadTexture("images/container_BW.png");
    unsigned int metalDiffuseMap = loadTexture("images/metal.png");
    unsigned int candelabraDiffuseMap = loadTexture("images/candelabra.png");
    unsigned int candleDiffuseMap = loadTexture("images/tulle.png");
    unsigned int candleSpecularMap = loadTexture("images/tulle.png");
    unsigned int headphoneDiffuseMap = loadTexture("images/headphones.png");
    unsigned int difuserDiffuseMap = loadTexture("images/difuser.png");
    unsigned int bambooDiffuseMap = loadTexture("images/bamboo.png");
    unsigned int copperDiffuseMap = loadTexture("images/copper.png");
    unsigned int copperSpecularMap = loadTexture("images/copper.png");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. 
        */
        // blueish directional light
        lightingShader.setVec3("dirLight.direction", -2.0f, 3.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.4f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.6f);
        lightingShader.setVec3("dirLight.specular", 0.4f, 0.4f, 0.6f);
        // White point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2 disabled
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[1].specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3 disabled
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[2].specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4 disabled
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("pointLights[3].specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight varying color based on number press
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", spotlight);
        lightingShader.setVec3("spotLight.diffuse", spotlight);
        lightingShader.setVec3("spotLight.specular", spotlight);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09);
        lightingShader.setFloat("spotLight.quadratic", 0.032);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render containers
        // render boxes for phone
        glBindVertexArray(VAO);
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the box
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.7f)); // move the box within the scene
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 72);

        // Add cylinder to VAO
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the cylinder
        model = glm::translate(model, glm::vec3(0.05f, 0.01f, 0.7f)); // move the cylinder within the scene
        lightingShader.setMat4("model", model);
        //Render the cylinder
        static_meshes_3D::Cylinder C(0.05f, 72, 0.0199f, true, true, true, false, false);
        C.render();

        // Add cylinder to VAO
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the cylinder
        model = glm::translate(model, glm::vec3(0.05f, 0.01f, 0.2f)); // move the cylinder within the scene
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C.render();

        // Add cylinder to VAO
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the cylinder
        model = glm::translate(model, glm::vec3(0.25f, 0.01f, 0.2f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C.render();

        // Add cylinder to VAO
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the cylinder
        model = glm::translate(model, glm::vec3(0.25f, 0.01f, 0.7f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C.render();


        // bind diffuse map for phone screen
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, screenSpecularMap);

        glBindVertexArray(VAO2);
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // rotate the plane
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.7f)); // move the plane within the scene
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // setup to draw plane for table top
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tableDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tableSpecularMap);

        // bind VAO for screen
        glBindVertexArray(VAO3);
        model = glm::mat4(1.0f);
        // Move the plane to fit with other objects
        model = glm::translate(model, glm::vec3(-2.0f, -0.001f, -0.5f));
        // Scal the plane to match the size of the table
        model = glm::scale(model, glm::vec3(7.0f, 1.0f, 4.0f));
        lightingShader.setMat4("model", model);
        // draw plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //bind diffuse texture for coasters
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, copperDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, copperSpecularMap);

        // bind VAO for coasters
        glBindVertexArray(coasterVAO);
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(-4.5f, 0.0f, -0.25f)); // move the box within the scene
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 30); // Render sides and bottom of a cube

        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(-3.7f, 0.0f, 0.5f)); // move the box within the scene
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate the plane
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 30);

        //bind diffuse texture for remote control
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, remoteDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, remoteSpecularMap);
        // bind VAO for Remote
        glBindVertexArray(VAO4);
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f)); // move the box within the scene
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36); // Draw remote body made of cube

        // Load texture 4 for the cylinders that make up the sides of the remote
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metalDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, remoteSpecularMap);

        glBindVertexArray(VAO4);
        // Create cylinder 2 for remote for rounded sides
        static_meshes_3D::Cylinder C2(0.02f, 72, 1.0f, true, true, true, true);
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-1.8f, 0.02f, 0.5f)); // move the cylinder within the scene
        lightingShader.setMat4("model", model);
        C2.render();
        // Add second Cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-2.0f, 0.02f, 0.5f)); // move the cylinder within the scene
        lightingShader.setMat4("model", model);
        C2.render();

        // Load texture for the body of the candelabra
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, candelabraDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        //Begin creating half Torus' for the candelabra base
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1, // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_TRUE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glFrontFace(GL_CW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.53f, 0.25f, -1.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate the plane
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, halfTorus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.8f, 0.25f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, halfTorus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-1.25f, 0.25f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, halfTorus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-2.2f, 0.25f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, halfTorus * 3); // 12*3 indices starting at 0 -> 12 triangles
        
        // Create full Torus' for the body of the candelabra
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        glVertexAttribPointer(0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
        glVertexAttribPointer(1, // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_TRUE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glFrontFace(GL_CW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(0.65f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-3.13f, 0.6f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(0.5f, 0.8f, 1.0f));
        model = glm::translate(model, glm::vec3(-2.3f, 0.64f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(0.5f, 0.8f, 1.0f));
        model = glm::translate(model, glm::vec3(-5.9f, 0.64f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(0.4f, 0.7f, 1.0f));
        model = glm::translate(model, glm::vec3(-8.85f, 0.64f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CCW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(0.4f, 0.7f, 1.0f));
        model = glm::translate(model, glm::vec3(-1.4f, 0.64f, -1.0f));
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus * 3); // 12*3 indices starting at 0 -> 12 triangles

        // Load texture for the headphones
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, headphoneDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Draw Torus for headphones
        // 1rst attribute buffer : vertices for Headphones
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer4);
        glVertexAttribPointer(0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer4);
        glVertexAttribPointer(1, // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_TRUE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glFrontFace(GL_CW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.9f, 0.21f, 0.29f));
        model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // rotate the plane
        lightingShader.setMat4("model", model);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus2 * 3); // 12*3 indices starting at 0 -> 12 triangles

        glFrontFace(GL_CW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.72f, 0.21f, 0.15f));
        model = glm::rotate(model, glm::radians(55.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // rotate the plane
        lightingShader.setMat4("model", model);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, torus2 * 3); // 12*3 indices starting at 0 -> 12 triangles

        // Create 3/4 Torus for the headphones loop
        // 1rst attribute buffer : vertices for Headphones
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
        glVertexAttribPointer(0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer3);
        glVertexAttribPointer(1, // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_TRUE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glFrontFace(GL_CW);

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        // Scale and translate the torus
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-1.0f, 0.12f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate the plane
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(-0.5f, 0.5f, 0.5f)); // rotate the plane
        lightingShader.setMat4("model", model);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, partTorus * 3); // 12*3 indices starting at 0 -> 12 triangles
    
        // setup to draw sphere for headphone body
        glBindVertexArray(sphereVAO);
        model = model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.98f, 0.21f, 0.34f));
        model = glm::rotate(model, glm::radians(49.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // rotate the plane
        model = glm::scale(model, glm::vec3(0.21f, 0.21f, 0.1f)); // Make it a smaller sphere
        lightingShader.setMat4("model", model);

        // draw sphere
        glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);

        // setup to draw sphere 2 for headphone body
        glBindVertexArray(sphereVAO);
        model = model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.65f, 0.21f, 0.1f));
        model = glm::rotate(model, glm::radians(62.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // rotate the plane
        model = glm::scale(model, glm::vec3(0.21f, 0.21f, 0.1f)); // Make it a smaller sphere
        lightingShader.setMat4("model", model);

        // draw sphere
        glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);

        //Draw platters for candelabra
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-0.56f, 0.735f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        static_meshes_3D::Cylinder C3(0.3f, 72, 0.03f, true, true, true, false, false);
        C3.render();

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-1.17f, 0.83f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C3.render();

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-2.04f, 1.0f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C3.render();

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-2.95f, 0.83f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C3.render();

        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-3.56f, 0.735f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C3.render();

        // Load texture for the cylinders for the candle holders on the candelabra
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, candleDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, candleSpecularMap);

        //Draw candels
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-0.56f, 1.008f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        static_meshes_3D::Cylinder C4(0.25f, 72, 0.5f, true, true, true, false, true);
        C4.render();
        //Draw candels
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-1.17f, 1.096f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C4.render();
        //Draw candels
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-2.04f, 1.266f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C4.render();
        //Draw candels
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-2.95f, 1.096f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C4.render();
        //Draw candels
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-3.56f, 1.008f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C4.render();


        // Load texture for the cones that make up the diffuser
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, difuserDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glBindVertexArray(coneVAO);
        //Draw platters for difuser
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.5f, 0.41f, -1.4f));
        lightingShader.setMat4("model", model);
        //Render the cone
        static_meshes_3D::Cone cone(0.2f, 72, 0.8f, true, true, true, false);
        cone.render();
        // cone number 2
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.5f, 0.41f, -1.4f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightingShader.setMat4("model", model);
        //Render the cone
        static_meshes_3D::Cone cone2(0.13f, 72, 0.8f, true, true, true, false);
        cone2.render();

        // Load texture for the cylinders that make the bamboo sticks in the diffuser
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bambooDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(coneVAO);
        //Rotate and translate the cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.59f, 1.0f, -1.4f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        static_meshes_3D::Cylinder C5(0.01f, 72, 0.6f, true, true, true, false, false);
        C5.render();
        //Rotate another translate the cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.44f, 1.0f, -1.37f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C5.render();
        //Rotate another translate the cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.56f, 1.0f, -1.35f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C5.render();
        //Rotate another translate the cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.43f, 1.0f, -1.5f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(-1.0f, 1.0f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C5.render();
        //Rotate another translate the cylinder
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(-4.44f, 1.0f, -1.32f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, -1.0f));
        lightingShader.setMat4("model", model);
        //Render the cylinder
        C5.render();

        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // we now draw the light.
        glBindVertexArray(lightCubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteVertexArrays(1, &VAO3);
    glDeleteVertexArrays(1, &VAO4);
    glDeleteVertexArrays(1, &coasterVAO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &sphereVAO2);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &coneVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &VBO3);
    glDeleteBuffers(1, &VBO4);
    glDeleteBuffers(1, &coasterVBO);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &vertexbuffer2);
    glDeleteBuffers(1, &vertexbuffer3);
    glDeleteBuffers(1, &vertexbuffer4);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &uvbuffer2);
    glDeleteBuffers(1, &uvbuffer3);
    glDeleteBuffers(1, &uvbuffer4);
    glDeleteBuffers(1, &coneVBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereVBO2);
    glDeleteBuffers(1, &lightCubeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    float cameraSpeed = defaultCameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, cameraSpeed);
    // If the projection is set to an ortho projection, change to perspective, otherwise, change to ortho
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        if (projection == glm::ortho(-(800.0f / scale), (800.0f / scale), (600.0f / scale), -(600.0f / scale), 0.1f, 100.0f)) {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        }
        else
        {
            projection = glm::ortho(-(800.0f / scale), (800.0f / scale), (600.0f / scale), -(600.0f / scale), 0.1f, 100.0f);
            glfwWaitEventsTimeout(0.7);
        }
    //Changes the color of the spotlight
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        spotlight = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        spotlight = glm::vec3(0.5f, 0.2f, 0.2f);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        spotlight = glm::vec3(0.2f, 0.5f, 0.2f);
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        spotlight = glm::vec3(0.2f, 0.2f, 0.5f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // change cameraSpeed variable based on mouse scroll
    defaultCameraSpeed += (float)yoffset;
    if (defaultCameraSpeed < 0.1f)
        defaultCameraSpeed = 0.1f;
    if (defaultCameraSpeed > 15.0f)
        defaultCameraSpeed = 15.0f;
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    // Generate a textureID to reference texture after creation
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // Load image
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        // Select image format type
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // Generate the texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // Set texture settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Free up memory
        stbi_image_free(data);
    }
    else
    {
        // Print error message if image fails to load
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
// Create vector coordinates for Torus 
void setCoords(double r, double c, int rSeg, int cSeg, int i, int j, 
    GLfloat* vertices, GLfloat* uv) {
    const double PI = 3.1415926535897932384626433832795;
    const double TAU = 2 * PI;

    double x = (c + r * cos(i * TAU / rSeg)) * cos(j * TAU / cSeg);
    double y = (c + r * cos(i * TAU / rSeg)) * sin(j * TAU / cSeg);
    double z = r * sin(i * TAU / rSeg);

    vertices[0] = 2 * x;
    vertices[1] = 2 * y;
    vertices[2] = 2 * z;

    uv[0] = i / (double)rSeg;
    uv[1] = j / (double)cSeg;
}

// Create Torus based on input size and scale, scale input creates partial Torus if less than 1
int createTorus(double r, double c, int rSeg, int cSeg, float scaler, GLfloat** vertices,
    GLfloat** uv) {
    // reduces scaler to 1, which is a full Torus, if it is larger than 1
    if (scaler > 1)
        scaler = 1;
    int count = rSeg * cSeg * 6;
    *vertices = (GLfloat*)malloc(count * 3 * sizeof(GLfloat));
    *uv = (GLfloat*)malloc(count * 2 * sizeof(GLfloat));

    for (int x = 0; x < cSeg; x++) { // through stripes
        for (int y = rSeg * (1 - scaler); y < rSeg; y++) { // through squares on stripe
            GLfloat* vertexPtr = *vertices + ((x * rSeg) + y) * 18;
            GLfloat* uvPtr = *uv + ((x * rSeg) + y) * 12;
            setCoords(r, c, rSeg, cSeg, x, y, vertexPtr + 0, uvPtr + 0);
            setCoords(r, c, rSeg, cSeg, x + 1, y, vertexPtr + 3, uvPtr + 2);
            setCoords(r, c, rSeg, cSeg, x, y + 1, vertexPtr + 6, uvPtr + 4);

            setCoords(r, c, rSeg, cSeg, x, y + 1, vertexPtr + 9, uvPtr + 6);
            setCoords(r, c, rSeg, cSeg, x + 1, y, vertexPtr + 12, uvPtr + 8);
            setCoords(r, c, rSeg, cSeg, x + 1, y + 1, vertexPtr + 15,
                uvPtr + 10);
        }
    }

    return count;
}