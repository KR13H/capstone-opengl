#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
const char * computeSrc = R"(
#version 430 core
layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba32f, binding = 0) uniform image2D img;
uniform float t;
void main () {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(p) / vec2(800.0 , 600.0);
    // float d = length(uv - vec2(0.5)); // far from center?
    // float wave = 0.5 + 0.5 * sin(d * 40.0 - t * 5.0); // rings that move outward 
    // imageStore(img, p, vec4(uv.x, uv.y, wave, 1.0));
    vec2 ndc = uv * 2.0 - 1.0;                       // 0..1  →  -1..1  (center = 0)
    ndc.x *= 800.0 / 600.0;                          // aspect ratio, like in perspective()
    vec3 rayDir = normalize(vec3(ndc, -1.0));        // camera looks down -z
    imageStore(img, p, vec4(rayDir * 0.5 + 0.5, 1.0));   // show the direction as a color
    }
)";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);   // TODO 1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);   // TODO 1
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Compute Shader", NULL, NULL);
    if (window == NULL) { std::cout << "Failed to create window" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to load GLAD" << std::endl; return -1; }

    // TODO 2: print the GPU name and OpenGL version here
    std::cout << glGetString(GL_RENDERER) << " | " << glGetString(GL_VERSION) << std::endl;

    unsigned int cs = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(cs, 1 ,&computeSrc, NULL);
    glCompileShader(cs);
    //check, print error code GPU prints
    int ok;
    char log[512];
    glGetShaderiv(cs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(cs, 512, NULL, log);
        std::cout << "COMPUTE SHADER ERROR:\n" << log << std::endl;
    }
    unsigned int computeProgram = glCreateProgram();
    glAttachShader(computeProgram, cs);
    glLinkProgram(computeProgram);

    unsigned int tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 800, 600, 0 , GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex, 0 , GL_FALSE, 0 , GL_WRITE_ONLY, GL_RGBA32F);


    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);


    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //clear above
        // GO! do the graphics thingies you want
        glUseProgram(computeProgram);
        // get time location from computeProgram and then sending it back
        int tLoc = glGetUniformLocation(computeProgram, "t");
        glUniform1f(tLoc, glfwGetTime());

        glDispatchCompute(100, 75, 1);   // 100 x 75 teams, GO
        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);   // wait until all painters finish

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); // copy FROM the canvas
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // copy TO the window
        glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}