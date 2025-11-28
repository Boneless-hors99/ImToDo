#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>   // On Linux this will use X11 internally
#include <stdio.h>

// Optional: if you needing a GL loader (glad, gl3w, etc.), include/init it here
// #include <glad/glad.h>

#include "App.h"

int main(){
    App app;
    app.Loop();
    app.End();
}