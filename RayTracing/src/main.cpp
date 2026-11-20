#include <glad/glad.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <chrono>

const int W = 800, H = 600;
const char* computeSrc = R"(
#version 430 core
layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba8, binding = 0) uniform image2D img;
uniform float t;
void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(p) / vec2(800.0, 600.0);
    vec2 ndc = uv * 2.0 - 1.0;                       // 0..1  →  -1..1  (center = 0)
    ndc.x *= 800.0 / 600.0;                          // aspect ratio, like in perspective()
    vec3 rayDir = normalize(vec3(ndc, -1.0));        // camera looks down -z

    vec3 center = vec3(0.0, 0.0, -3.0);
    float radius = 1.0;
    vec3 oc = -center;
    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;

    vec4 color;
    if(h >= 0.0) {
    float dist = -b - sqrt(h); // how far along the ray the hit is
    vec3 hitPoint = dist * rayDir; // exact 3d point on the ball
    vec3 normal = normalize(hitPoint - center); // direction staright out of surface
    //color = vec4(1.0, 0.5, 0.2, 1.0); // orange color
    //color = vec4(normal * 0.5 + 0.5, 1.0);
    //vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 lightDir = normalize(vec3(cos(t), 1.0, sin(t)));
    float diffuse = max(dot(normal, lightDir),0.0);
    color = vec4(diffuse * vec3(1.0, 0.5, 0.2) , 1.0);
    } else {
     color = vec4(rayDir * 0.5 + 0.5,1.0);
    }
    imageStore(img, p, color);   // show the direction as a color
}
)";

int main() {
    // ---- 1. GPU side: OpenGL on the NVIDIA card, no window (EGL) ----
    auto queryDevices = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
    auto getPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    EGLDeviceEXT devices[8]; EGLint numDevices = 0;
    queryDevices(8, devices, &numDevices);
    EGLDisplay dpy = getPlatformDisplay(EGL_PLATFORM_DEVICE_EXT, devices[0], NULL);
    if (!eglInitialize(dpy, NULL, NULL)) { std::cout << "EGL init failed\n"; return -1; }
    eglBindAPI(EGL_OPENGL_API);
    EGLint cfgAttr[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE };
    EGLConfig cfg; EGLint n; eglChooseConfig(dpy, cfgAttr, &cfg, 1, &n);
    EGLint ctxAttr[] = { EGL_CONTEXT_MAJOR_VERSION, 4, EGL_CONTEXT_MINOR_VERSION, 3,
                         EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, EGL_NONE };
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctxAttr);
    if (ctx == EGL_NO_CONTEXT) { std::cout << "Context failed\n"; return -1; }
    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx);
    if (!gladLoadGLLoader((GLADloadproc)eglGetProcAddress)) { std::cout << "GLAD failed\n"; return -1; }
    std::cout << "GPU: " << glGetString(GL_RENDERER) << " | " << glGetString(GL_VERSION) << std::endl;

    // canvas + compute shader (same as your ComputeShaders project)
    unsigned int tex; glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    unsigned int cs = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(cs, 1, &computeSrc, NULL); glCompileShader(cs);
    int ok; char log[512]; glGetShaderiv(cs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(cs, 512, NULL, log); std::cout << "COMPUTE SHADER ERROR:\n" << log; }
    unsigned int prog = glCreateProgram(); glAttachShader(prog, cs); glLinkProgram(prog);
    int tLoc = glGetUniformLocation(prog, "t");

    // ---- 2. Screen side: a plain X11 window (travels over ssh -Y to XQuartz) ----
    Display* x = XOpenDisplay(NULL);
    if (!x) { std::cout << "No X display. Connect with: ssh -Y csil-gpu\n"; return -1; }
    Window win = XCreateSimpleWindow(x, DefaultRootWindow(x), 0, 0, W, H, 0, 0, 0);
    XStoreName(x, win, "Compute Shader (RTX -> Mac)");
    XSelectInput(x, win, KeyPressMask);
    Atom closeMsg = XInternAtom(x, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x, win, &closeMsg, 1);
    XMapWindow(x, win);
    GC gc = DefaultGC(x, DefaultScreen(x));
    std::vector<unsigned char> rgba(W * H * 4), bgrx(W * H * 4);
    XImage* image = XCreateImage(x, DefaultVisual(x, DefaultScreen(x)), 24, ZPixmap, 0,
                                 (char*)bgrx.data(), W, H, 32, 0);

    auto start = std::chrono::steady_clock::now();
    bool running = true;
    while (running) {
        while (XPending(x)) {                        // ESC or the close button quits
            XEvent e; XNextEvent(x, &e);
            if (e.type == KeyPress && XLookupKeysym(&e.xkey, 0) == XK_Escape) running = false;
            if (e.type == ClientMessage && (Atom)e.xclient.data.l[0] == closeMsg) running = false;
        }
        float t = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();

        glUseProgram(prog);                          // GPU paints the frame
        glUniform1f(tLoc, t);
        glDispatchCompute(W / 8, H / 8, 1);
        glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());   // GPU -> CPU
        for (int y = 0; y < H; y++)                  // flip rows + RGBA -> BGRX for X11
            for (int i = 0; i < W; i++) {
                unsigned char* s = &rgba[((H - 1 - y) * W + i) * 4];
                unsigned char* d = &bgrx[(y * W + i) * 4];
                d[0] = s[2]; d[1] = s[1]; d[2] = s[0]; d[3] = 0;
            }
        XPutImage(x, win, gc, image, 0, 0, 0, 0, W, H);   // send the picture to your Mac
        XFlush(x);
    }
    image->data = NULL; XDestroyImage(image);
    XCloseDisplay(x);
}
