#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;
uniform float xOffset;
uniform float yOffset;
void main() {
    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);
    vColor = gl_Position.xyz * 0.5, 0.5;
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
        -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f, //bottom left
        0.5f,  -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, //bottom right
        0.0f,   0.5f,  0.0f, 0.0f, 0.0f, 1.0f //top
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

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // pick the background color
        glClear(GL_COLOR_BUFFER_BIT);           // paint the screen with it

        //linking below 
        glUseProgram(shaderProgram); // draw triangle 
        
        // Make the color change with fragment shader
        // //uniform for ourColor
        // int loc = glGetUniformLocation(shaderProgram, "ourColor"); // gives location of ourcolor -> which is uniform declared at top
        // //Animate the color of triangle
        float t = glfwGetTime(); //seconds since prgram started
        // float wave = sin(t*t) /2.0f+0.5f; // swing btw 0 & 1 smoothly
        // glUniform4f(loc, 0.3f,wave,0.6f,1.0f); //purple color, a set to 1.0. 

        //Make the tranlge move left and right with Vertex shader
        int loc2 = glGetUniformLocation(shaderProgram, "xOffset");
        float wave2 = sin(t)*0.5f;
        glUniform1f(loc2, wave2);
        //Make the tranlge move up and down with Vertex shader
        int loc3 = glGetUniformLocation(shaderProgram, "yOffset");
        float wave3 = cos(t)*0.5f;
        glUniform1f(loc3, wave3);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //linking ends above

        glfwSwapBuffers(window);                // show the result
        glfwPollEvents();                       // check for clicks and keys
    }
    
    glfwTerminate();
    return 0;
}