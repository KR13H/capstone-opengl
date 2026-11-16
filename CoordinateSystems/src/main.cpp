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

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // pick the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           // paint the screen with it
        //linking below 
        glUseProgram(shaderProgram); // draw triangle 
        
        // Make the color change with fragment shader
        float t = glfwGetTime(); //seconds since prgram started
        float s = sin(t) * 0.25f + 0.75f;
        for (int i = 0 ; i < 3 ; i++) {
            // //trans = glm::rotate(trans, t, glm::vec3(0.0f, 0.0f, 1.0f));
            // glm::mat4 model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(1.0f, 0.5f, 0.0f)); 
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, t, glm::vec3(1.0f, 0.5f, 0.0f));
            //glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, -3.0f)); // push the trianlge 3 units away from camera
            glm::mat4 view = glm::lookAt(
                glm::vec3(2.0f, 0.0f, 5.0f), //eye: camera is 3 units away
                glm::vec3(0.0f, 0.0f, 0.0f), //looking at center
                glm::vec3(0.0f, 1.0f, 0.0f) //up: the sky is up
            );
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); // 45 deg of view window's shape btw 0.1 to 100 units away

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
        glfwSwapBuffers(window);                // show the result
        glfwPollEvents();                       // check for clicks and keys
    }
    
    glfwTerminate();
    return 0;
}