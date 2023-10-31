//#define GLFW_INCLUDE_ES3
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_glfw.h"
//#include "imgui/imgui_impl_opengl3.h"
//
//#include "Renderer.h"
//#include "VertexBuffer.h"
//#include "IndexBuffer.h"
//#include "VertexBufferLayout.h"
//#include "VertexArray.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "Object.h"
//#include "Light.h"
//
//std::vector<float> floorSquare = {
//        -.5f, -.5f, -.5f, .0f, -1.f, .0f, 0.f, 0.f,//none //0
//         .5f, -.5f, -.5f, .0f, -1.f, .0f, 1.f, 0.f,//x    //1
//        -.5f, -.5f,  .5f, .0f, -1.f, .0f, 0.f, 1.f,//z    //2
//         .5f, -.5f,  .5f, .0f, -1.f, .0f, 1.f, 1.f //xz   //3
//};
//std::vector<unsigned int> floorIndices = {
//    0,1,2,
//    1,2,3
//};
//std::vector<float> defaultCube = {
//    -.5f, -.5f, -.5f, 0.f, 0.f,//none //0
//        .5f, -.5f, -.5f, 1.f, 0.f,//x    //1
//    -.5f, -.5f,  .5f, 1.f, 0.f,//z    //2
//    -.5f,  .5f, -.5f, 0.f, 1.f,//y    //3
//        .5f, -.5f,  .5f, 0.f, 0.f,//xz   //4
//        .5f,  .5f, -.5f, 1.f, 1.f,//xy   //5
//    -.5f,  .5f,  .5f, 0.f, 1.f,//yz   //6
//        .5f,  .5f,  .5f, 1.f, 1.f//xyz   //7
//};
//std::vector<float> cubeNoTextures = {
//    -.5f, -.5f, -.5f,//none //0
//        .5f, -.5f, -.5f,//x    //1
//    -.5f, -.5f,  .5f,//z    //2
//    -.5f,  .5f, -.5f,//y    //3
//        .5f, -.5f,  .5f,//xz   //4
//        .5f,  .5f, -.5f,//xy   //5
//    -.5f,  .5f,  .5f,//yz   //6
//        .5f,  .5f,  .5f //xyz   //7
//};
//std::vector<float> colorCube = {
//    -.5f, -.5f, -.5f, .1f, .2f, .3f, //none //0
//        .5f, -.5f, -.5f, .2f, .3f, .1f, //x    //1
//    -.5f, -.5f,  .5f, .3f, .2f, .1f,//z    //2
//    -.5f,  .5f, -.5f, .7f, .3f, .4f,//y    //3
//        .5f, -.5f,  .5f, .0f, .6f, .7f,//xz   //4
//        .5f,  .5f, -.5f, .3f, .3f, .3f,//xy   //5
//    -.5f,  .5f,  .5f, .3f, .0f, .9f,//yz   //6
//        .5f,  .5f,  .5f, .5f, .8f, .1f //xyz   //7
//};
//std::vector<unsigned int> cubeIndices = {
//    0,1,5,
//    0,3,5,
//    0,2,6,
//    0,3,6,
//    0,1,4,
//    0,2,4,
//    3,6,7,
//    3,5,7,
//    1,5,7,
//    1,4,7,
//    2,4,7,
//    2,6,7
//};
//
//void createFloor(std::vector<Object>& objects, Mesh* square, Shader* shader, float width, float height, int n)
//{
//    float blockWidth = width / n,
//        blockHeight = height / n,
//        startHeight = -height / 2,
//        startWidth = -width / 2,
//        yValue = 20;
//    glm::vec3 scale(blockWidth, 1, blockHeight);
//    int end = objects.size();
//    for (int i = 0;i < n;i++)
//    {
//        for (int u = 0;u < n;u++)
//        {
//            glm::vec3 pos(startWidth + i * blockWidth, yValue, startHeight + u * blockHeight);
//            objects.push_back(Object(square, shader, pos));
//            objects[end++].setScale(scale);
//        }
//    }
//}
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
//    glfwSetCursorPos(window, 0.0, 0.0);
//
//    ImGui::CreateContext();
//    ImGui::StyleColorsDark();
//    //ImGui::StyleColorsLight();
//
//    // Setup Platform/Renderer backends
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 130");
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
//
//    {
//        //Mesh cube(defaultCube, cubeIndices, 3, false, true);
//        //Mesh colorCubeMesh(colorCube, cubeIndices, 3, true, false);
//        Mesh square(floorSquare, floorIndices, 3);
//        Mesh capsule("res/meshes/capsule.obj", 3, true,false);
//        Mesh cone("res/meshes/cone.obj", 3);
//        Mesh cube("res/meshes/cube.obj", 3);
//        Mesh cylinder("res/meshes/cylinder.obj", 3);
//        Mesh icosahedron("res/meshes/icosahedron.obj", 3);
//        Mesh quartz("res/meshes/quartz.obj", 3);
//        Mesh sphere("res/meshes/sphere.obj", 3);
//        
//        Shader textureShader("res/shaders/cube.shader");
//        Shader defaultShader("res/shaders/default.shader");
//        Shader colorShader("res/shaders/color.shader");
//        Shader lightShader("res/shaders/light.shader");
//        Shader screenShader("res/shaders/screen.shader");
//        Shader depthShader("res/shaders/pointLightDepth.shader");
//
//        Texture cloud("res/textures/LCACloud.png");
//        Texture sparks("res/textures/LCASparks.png");
//        Texture color;
//        Texture depth;
//
//        Light sun(glm::vec3(-10), glm::vec3(0, 0, -1), glm::vec3(1.f, 1.f, 1.f), 10.f, Light::LightType::Point, &depthShader);
//
//        std::vector<Object> objList = {
//            //Object(&cube, &shader, &sparks, glm::vec3(1.f, 2.f, 5.f)),
//            Object(&cube, &textureShader, glm::vec3(2.f, 3.f, 0.f)),
//            Object(&cone, &lightShader, glm::vec3(1)),
//            Object(&cube, &lightShader, glm::vec3(2)),
//            Object(&cylinder, &lightShader, glm::vec3(3)),
//            Object(&capsule, &lightShader, glm::vec3(4))
//        };
//        createFloor(objList, &square, &defaultShader, 400, 400, 6);
//        objList[0].setTexture(sun.texture);
//        Object screen(&square, &screenShader, &color, glm::vec3(100));
//
//        FrameBuffer PostProcessingFBO(&color, &depth, width, height);
//        Renderer renderer(width, height);
//        Renderer internalRenderer(&PostProcessingFBO);
//
//        internalRenderer.camera.setPosition(glm::vec3(-4.f, 4.f, 4.f));
//        internalRenderer.camera.setOrientation(glm::vec3(1.f, -1.f, -1.f));
//
//        sun.clear();
//        sun.draw(objList);
//
//        lightShader.bind();
//        lightShader.setLight(sun);
//        
//        float r = .0f, dr = .002f;
//        while (!glfwWindowShouldClose(window))
//        {
//            ImGui_ImplOpenGL3_NewFrame();
//            ImGui_ImplGlfw_NewFrame();
//            ImGui::NewFrame();
//
//            r += dr;
//            if (r > 1 || r < 0)
//                dr = -dr;
//            {
//                ImGui::Begin("Editor");
//                ImGui::SliderFloat("light intensity", &sun.intensity, 0.0f, 200.0f);
//                ImGui::ColorEdit3("light color", (float*)&sun.color.r);
//                ImGui::SliderFloat3("light position", &sun.position.x, -50.0f, 50.0f);
//                ImGui::End();
//            }
//            sun.clear();
//            sun.draw(objList);
//
//            lightShader.bind();
//            lightShader.setLight(sun);
//
//            internalRenderer.clear();
//            internalRenderer.draw(objList);
//
//            renderer.clear();
//            renderer.draw(screen);
//
//            objList[1].translate(glm::vec3(0, 10 * dr, -10 * dr));
//            internalRenderer.camera.getInput(window, width, height);
//
//            ImGui::Render();
//            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//            glfwSwapBuffers(window);
//            glfwPollEvents();
//        }
//    }
//    GLAllErrors();
//
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//    glfwTerminate();
//    return 0;
//}
