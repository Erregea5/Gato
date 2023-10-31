//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_glfw.h"
//#include "imgui/imgui_impl_opengl3.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//
//#include "Renderer.h"
//#include "neural/network.hpp"
//#include "neural/modules.hpp"
//
//static bool init(GLFWwindow* window)
//{
//    if (!window)
//    {
//        glfwTerminate();
//        return 0;
//    }
//
//    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1);
//
//    ImGui::CreateContext();
//    ImGui::StyleColorsDark();
//    //ImGui::StyleColorsLight();
//
//    // Setup Platform/Renderer backends
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 130");
//
//
//    GLenum err = glewInit();
//    if (GLEW_OK != err)
//    {
//        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
//        return 0;
//    }
//    return 1;
//}
//
//int main(void)
//{
//    int width = 640,
//        height = 480;
//    if (!glfwInit())
//        return 1;
//    GLFWwindow* window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
//    if (!init(window))
//        return -1;
//{
//    Renderer renderer(width,height);
//    renderer.camera.setOrientation(glm::vec3(0.f, 0.f, 1.f));
//    renderer.camera.setPosition(glm::vec3(0.f, 0.f, -.5f));
//
//    ImGuiIO& io = ImGui::GetIO();
//    
//    bool show_demo_window = false;
//    float r = .0f, dr = .01f;
//    glm::vec3 clearColor(0);
//
//    std::vector<NeuralNetwork<Activations::LeakyReLU>> networks;
//    while (!glfwWindowShouldClose(window))
//    {
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        {
//            static float f[] = { 0.1f,10.f ,(float)640*10/480,(float)10};
//            static int counter = 0;
//
//            ImGui::Begin("Network Builder!");
//
//            ImGui::Text("Build your Networks");
//            if (ImGui::BeginMenuBar())
//            {
//                if (ImGui::BeginMenu("Your Networks"))
//                {
//                    for (auto& network : networks) 
//                    {
//                        if (ImGui::MenuItem(network.folder.c_str())) { /* Do stuff */ }
//                    }
//                    if (ImGui::MenuItem("New Networks")) {  }
//                    ImGui::EndMenu();
//                }
//                ImGui::EndMenuBar();
//            }
//            ImGui::Checkbox("Demo Window", &show_demo_window);
//
//            ImGui::SliderFloat4("float", f, -100.0f, 100.0f);
//            
//            renderer.ortho=glm::ortho(-f[2], f[2], -f[3], f[3], f[0], f[1]);
//            ImGui::ColorEdit3("clear color", (float*)&clearColor.r);
//
//            if (ImGui::Button("Button"))
//                counter++;
//            ImGui::SameLine();
//            ImGui::Text("counter = %d", counter);
//
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
//
//            ImGui::End();
//        }
//
//        if (show_demo_window)
//            ImGui::ShowDemoWindow(&show_demo_window);
//        renderer.clear(clearColor);
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//}
//    GLAllErrors();
//
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}