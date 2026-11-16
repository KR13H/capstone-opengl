#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//shaders
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view* model *vec4(aPos, 1.0); 
    vColor = aColor; 
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 vColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

float fov = 45.0f;
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
    fov -= (float)yoffset; // scroll up is zoom in here
    // if (fov < 1.0f) fov = 1.0f;
    // if (fov > 45.0f) fov = 45.0f;
}
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f); // the direction it faces (into the screen)
float yaw = -90.0f; // facing into screen -> -90
float pitch = 0.0f;
float lastX = 400, lastY = 300; //last mouse position (center)
bool firstMouse = true;


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float dx = (xpos - lastX) * 0.1f;
    float dy = (lastY - ypos) * 0.1f;
    lastX = xpos;
    lastY = ypos;

    yaw += dx;
    pitch += dy;
    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(dir);
}

//cpp
int main () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800,600, "Hello Triangle", NULL, NULL);

    if (window == NULL) {
        std::cout <<"Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // hides and locks the mouse
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout <<"Failed to load GLAD" << std::endl;
        return -1;
    }
    // make a triangle 

   float vertices[] = {
    // x     y     z      r  g  b
    -0.5f,-0.5f, 0.5f,  1,0,0,   0.5f,-0.5f, 0.5f,  1,0,0,   0.5f, 0.5f, 0.5f,  1,0,0,   // front (red)
     0.5f, 0.5f, 0.5f,  1,0,0,  -0.5f, 0.5f, 0.5f,  1,0,0,  -0.5f,-0.5f, 0.5f,  1,0,0,
    -0.5f,-0.5f,-0.5f,  0,1,0,   0.5f,-0.5f,-0.5f,  0,1,0,   0.5f, 0.5f,-0.5f,  0,1,0,   // back (green)
     0.5f, 0.5f,-0.5f,  0,1,0,  -0.5f, 0.5f,-0.5f,  0,1,0,  -0.5f,-0.5f,-0.5f,  0,1,0,
    -0.5f, 0.5f, 0.5f,  0,0,1,  -0.5f, 0.5f,-0.5f,  0,0,1,  -0.5f,-0.5f,-0.5f,  0,0,1,   // left (blue)
    -0.5f,-0.5f,-0.5f,  0,0,1,  -0.5f,-0.5f, 0.5f,  0,0,1,  -0.5f, 0.5f, 0.5f,  0,0,1,
     0.5f, 0.5f, 0.5f,  1,1,0,   0.5f, 0.5f,-0.5f,  1,1,0,   0.5f,-0.5f,-0.5f,  1,1,0,   // right (yellow)
     0.5f,-0.5f,-0.5f,  1,1,0,   0.5f,-0.5f, 0.5f,  1,1,0,   0.5f, 0.5f, 0.5f,  1,1,0,
    -0.5f,-0.5f,-0.5f,  1,0,1,   0.5f,-0.5f,-0.5f,  1,0,1,   0.5f,-0.5f, 0.5f,  1,0,1,   // bottom (pink)
     0.5f,-0.5f, 0.5f,  1,0,1,  -0.5f,-0.5f, 0.5f,  1,0,1,  -0.5f,-0.5f,-0.5f,  1,0,1,
    -0.5f, 0.5f,-0.5f,  0,1,1,   0.5f, 0.5f,-0.5f,  0,1,1,   0.5f, 0.5f, 0.5f,  0,1,1,   // top (cyan)
     0.5f, 0.5f, 0.5f,  0,1,1,  -0.5f, 0.5f, 0.5f,  0,1,1,  -0.5f, 0.5f,-0.5f,  0,1,1,
  };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  1.0f, -4.0f),
        glm::vec3(-1.5f, -1.0f, -2.5f),
    };

    // VAO has to exit first so it can record the buffer setup
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // send vertices to GPU
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices into buffer

    //slot 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // layout descripton 
    glEnableVertexAttribArray(0);
    //slot 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // 1st step create vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int ok;
    char log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        std::cout << "VERTEX SHADER ERROR:\n" << log << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        std::cout << "FRAGMENT SHADER ERROR:\n" << log << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram(); //copied. linking needs to be learnt. 
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 6.0f); // where the camera stands
    //comFront moved outside main, to make it global
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camRight = glm::normalize(glm::cross(camFront, camUp)); // normalize makes the length exactly 1

    float angle = 0.0f; // where around the circle
    float height = 0.0f; // how high the camera is
    float radius = 6.0f; // how far from the center
    int mode = 1;   // 1 = fly, 2 = orbit
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // pick the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           // paint the screen with it
        //linking below 
        glUseProgram(shaderProgram); // draw triangle 
        glm::vec3 camRight = glm::normalize(glm::cross(camFront, camUp)); // normalize makes the length exactly 1
        //key press controlling the camera
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camPos += 0.1f * camFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camPos -= 0.1f * camFront;
        // key A
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camPos -= 0.1f * camRight;
        // key D
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camPos += 0.1f * camRight;
        
        // around the cube
        //left key
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            angle -= 0.02f;
        // right key
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            angle += 0.02f;
        // up key
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            height += 0.05f;
        //down key
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            height -= 0.05f;
        // switch btw modes 
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = 1;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = 2;

        // Make the color change with fragment shader
        float t = glfwGetTime(); //seconds since prgram started

        //glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f); // 45 deg of view window's shape btw 0.1 to 100 units away
        // aroud in circle (prof req)
        // glm::vec3 eye = glm::vec3(sin(angle) * radius, height, cos(angle) * radius);
        // glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), camUp);
        glm::mat4 view;   // declare it first, empty

        if (mode == 1) {
            view = glm::lookAt(camPos, camPos + camFront, camUp);            // fly
        } else {
            glm::vec3 eye = glm::vec3(sin(angle) * radius, height, cos(angle) * radius);
            view = glm::lookAt(eye, glm::vec3(0.0f), camUp);                 // orbit
        }

        for (int i = 0 ; i < 3 ; i++) {
            // //trans = glm::rotate(trans, t, glm::vec3(0.0f, 0.0f, 1.0f));
            // glm::mat4 model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(1.0f, 0.5f, 0.0f)); 
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, t/10000000000, glm::vec3(1.0f, 0.5f, 0.0f));
            //glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, -3.0f)); // push the trianlge 3 units away from camera
           
            int tLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(tLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
            int tLoc2 = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(tLoc2, 1, GL_FALSE, glm::value_ptr(view));

            int tLoc3 = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(tLoc3, 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //linking ends above
        }    
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        glfwSwapBuffers(window);                // show the result
        glfwPollEvents();                       // check for clicks and keys
    }
    
    glfwTerminate();
    return 0;
}