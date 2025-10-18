#include<chrono>
#include<iostream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "shader.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"
#include "model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    // Initing GLFW and setting the openGL versions
    // Also explicitly telling the glfw that we want to use the core profile of openGL (the modern functions)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Making a new window and setting the current context to that window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "og", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    // initing GLAD
    // GLAD containes the function pointers of glfw
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setting the size of openGL viewport
    // from bottom left corner to top right corner
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    // Function to set callback to be executed when the window size changes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    //stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    std::cout << "Initing Shader" << std::endl; std::cout.flush();
    Shader ourShader("model_loading.vs", "model_loading.fs");
    std::cout << "Shader initiated" << std::endl; std::cout.flush();
    char path[] = "D:/OpenGL/models/computer/computer.obj";
    std::cout << "Initing Model" << std::endl; std::cout.flush();
    Model ourModel(path);
    std::cout << "Model initiated" << std::endl; std::cout.flush();
    

    // The render loop
    // One iter = 1 Frame
    while (!glfwWindowShouldClose(window)) {
        // measure time took to render the last frame
        // so that we can normalize the camera speed
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // For measuing FPS
        auto start = std::chrono::high_resolution_clock::now();
        // setting the color of viewport
        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        // clearing the previous set color
        // it will set the new color to the one we set above
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);


  //      glBindVertexArray(VAO);
  //      // Sending data from cpu to gpu (shaders)
  //      lightShader.use();
        ourShader.use();
        ourShader.setVec3("dirLight.direction", glm::vec3(- 1.0f, -0.2f, -0.3f));
        ourShader.setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        ourShader.setVec3("dirLight.specular", glm::vec3(0.3f, 0.3f, 0.3f));
  //      // point light 1
        ourShader.setVec3("pointLight.position", glm::vec3(0.7f, 0.2f, 2.0f));
        ourShader.setVec3("pointLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        ourShader.setVec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setFloat("pointLight.constant", 1.0f);
        ourShader.setFloat("pointLight.linear", 0.09f);
        ourShader.setFloat("pointLights.quadratic", 0.032f);
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(10.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		ourShader.setVec3("viewPos", camera.Position);


		ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        // Swaping the front and rear buffer
        // This is where the screen is updated
        glfwSwapBuffers(window);
        // Will listen to interactive events
        glfwPollEvents();

        // Measure FPS
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        //std::cout << 1000 / duration.count() << std::endl;
    }

    glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}