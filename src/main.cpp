#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>   // On Linux this will use X11 internally
#include <stdio.h>

// Optional: if you needing a GL loader (glad, gl3w, etc.), include/init it here
// #include <glad/glad.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    // GLFW init with X11 otherwise mouse position problems on my machine
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Setup OpenGL context (3.3 core is common)
    const char* glsl_version = "#version 130";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Uncomment on macOS

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Docking Demo - imtodo", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // If using a GL loader (glad/gl3w/etc.), init it *here*.
    // Example with glad (if you added it):
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     fprintf(stderr, "Failed to initialize GL loader!\n");
    //     return 1;
    // }
    // Have this here just in case I need to use it like I've done in the past

    // ----------------------------------------------------
    // ImGui context + docking
    // ----------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Enable docking + multi-viewport (optional but nice and I like keyboard nav)
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; Just because I'll later make it an option

    ImGui::StyleColorsDark();

    // Style tweaks for multi-viewport (no rounded corners, same bg)
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Backend init
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ----------------------------------------------------
    // Main loop
    // ----------------------------------------------------
    bool show_demo_window = true;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Fullscreen dockspace window ---
        {
            static bool dockspace_open = true;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("DockSpaceHost", &dockspace_open, window_flags);

            if (!opt_padding)
                ImGui::PopStyleVar();
            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Dockspace
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyMainDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            }

            // Simple menu bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Quit"))
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Show Demo Window", nullptr, &show_demo_window);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::End(); // DockSpaceHost
        }

        // --- ImGui Demo Window (shows all widgets & docking) ---
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // ----------------------------------------------------
        // Rendering
        // ----------------------------------------------------
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Multi-viewport: render other platform windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_ctx = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_ctx);
        }

        glfwSwapBuffers(window);
    }

    // ----------------------------------------------------
    // Cleanup
    // ----------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}