#ifndef IMTODO
#define IMTODO 

#include <GLFW/glfw3.h>

class App{
    private:
    GLFWwindow* m_window;
    public:
    App();
    void Loop();
    void End();
};

#endif // !IMTODO