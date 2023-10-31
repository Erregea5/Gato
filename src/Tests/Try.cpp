//#ifdef __EMSCRIPTEN__
//    #define GLFW_INCLUDE_ES3
//    #include <emscripten.h>
//#else
//    #include <GL/glew.h>
//#endif
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <functional>
//
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
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
//std::vector<float> defaultCube = {
//            -.5f, -.5f, -.5f, 0.f, 0.f,//none //0
//             .5f, -.5f, -.5f, 1.f, 0.f,//x    //1
//            -.5f, -.5f,  .5f, 1.f, 0.f,//z    //2
//            -.5f,  .5f, -.5f, 0.f, 1.f,//y    //3
//             .5f, -.5f,  .5f, 0.f, 0.f,//xz   //4
//             .5f,  .5f, -.5f, 1.f, 1.f,//xy   //5
//            -.5f,  .5f,  .5f, 0.f, 1.f,//yz   //6
//             .5f,  .5f,  .5f, 1.f, 1.f//xyz   //7
//};
//std::vector<float> noTextures = {
//    -.5f, -.5f, -.5f,//none //0
//     .5f, -.5f, -.5f,//x    //1
//    -.5f, -.5f,  .5f,//z    //2
//    -.5f,  .5f, -.5f,//y    //3
//     .5f, -.5f,  .5f,//xz   //4
//     .5f,  .5f, -.5f,//xy   //5
//    -.5f,  .5f,  .5f,//yz   //6
//     .5f,  .5f,  .5f //xyz   //7
//};
//std::vector<unsigned int> indices = {
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
//std::vector<std::string> skyBoxPaths = {
//    "res/textures/graycloud_lf.jpg",//POSITIVE_X
//    "res/textures/graycloud_rt.jpg",//NEGATIVE_X
//    "res/textures/graycloud_dn.jpg",//POSITIVE_Y
//    "res/textures/graycloud_up.jpg",//NEGATIVE_Y
//    "res/textures/graycloud_ft.jpg",//POSITIVE_Z
//    "res/textures/graycloud_bk.jpg" //NEGATIVE_Z
//};
//
//std::function<void()> loop;
//void main_loop() 
//{
//    loop();
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
//#ifdef __EMSCRIPTEN__
//    GLenum err = GLEW_OK;
//#else
//    GLenum err = glewInit();
//#endif
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
//        Graphics::Mesh cube(defaultCube, indices,3, false, true);
//        Graphics::Mesh man("res/meshes/FinalBaseMesh.obj", 3,true, false);
//        Graphics::Mesh square(floorSquare, floorIndices, 3, true, true);
//        Graphics::Mesh baseballBat("res/meshes/baseballBat.obj", 3);
//
//        Graphics::Shader cubeShader("res/shaders/cube.shader");
//        std::vector<Graphics::Shader> lightShaders = Graphics::Shader::createShaders(2,"res/shaders/light.shader");
//        Graphics::Shader directDepthShader("res/shaders/directLightDepth.shader");
//       // Shader pointDepthShader("res/shaders/pointLightDepth.shader");
//        Graphics::Shader screenShader("res/shaders/screen.shader");
//        //Shader skyBoxShader("res/shaders/skybox.shader");
//
//        Graphics::Texture cloud("res/textures/LCACloud.png");
//        //Texture skyCloud(skyBoxPaths);
//        Graphics::Texture sparks("res/textures/LCASparks.png");
//        Graphics::Texture color;
//        Graphics::Texture depth;glm::mat4 h(0);
//
//        std::vector<Object> objs = {
//            Object(&cube, &cubeShader, &cloud, glm::vec3(-1,0,0)),
//            Object(&cube, &cubeShader, &sparks, glm::vec3(1.f,2.f,5.f)),
//            Object(&baseballBat, &lightShaders[0],&cloud, glm::vec3(-1.f, -10.f, -2.f)),
//            Object(&man, &lightShaders[1], glm::vec3(-1,-1,-10)),
//            Object(&cube, &cubeShader, &cloud, glm::vec3(1)),
//            //Object(&cube, &skyBoxShader, glm::vec3(0)),
//            //Object(&cube, &skyBoxShader, &skyCloud, glm::vec3(0))
//        };
//        Object screen(&square, &screenShader, &color, glm::vec3(0));
//        
//        Graphics::Light sun(glm::vec3(-.5,12,20), glm::vec3(0,0,-1.5), glm::vec3(1), .5f, Graphics::Light::LightType::Direct,&directDepthShader);
//        //Light point(glm::vec3(0, 0, 3), glm::vec3(0,0,1), glm::vec3(.0, .3, .4), 25.f, Light::LightType::Point,&pointDepthShader);
//        
//        //objs[5].setTexture(point.texture);
//        //objs[6].scale(160);
//        
//        Graphics::FrameBuffer PostProcessingFBO(&color, &depth, width, height);
//        Graphics::Renderer renderer(width,height);
//        Graphics::Renderer internalRenderer(&PostProcessingFBO);
//        internalRenderer.camera.transform.setPosition(glm::vec3(-4.f, 4.f, 4.f));
//        internalRenderer.camera.transform.setOrientation(glm::vec3(1.f, -1.f, -1.f));
//
//        //screen.bind();screen.setTexture(sun.texture);
//        
//        sun.clear();
//        sun.draw(objs);
//        //point.clear();
//        //point.draw(objs);
//
//        for (auto& lightShader : lightShaders)
//        {
//            lightShader.bind();
//            lightShader.setLight(sun);
//            //lightShader.setLight(point);
//        }
//        
//        float r = .0f, dr = .003f;
//        loop = [&]
//        {
//            r += dr;
//            if (r > 1 || r < 0)
//            {
//                dr = -dr;
//                auto& c = internalRenderer.camera;
//                
//            }
//
//
//            internalRenderer.clear();
//            internalRenderer.draw(objs);
//
//            renderer.clear();
//            renderer.draw(screen);
//
//            internalRenderer.camera.getInput(window,width,height,1);
//            //objs[1].translate(glm::vec3(0, 10 * dr, -10 * dr));
//            //point.setPosition(-internalRenderer.camera.position);
//            //point.translate(glm::vec3(0, 15 * dr, 0));
//            //objs[4].setPosition(glm::vec3(point.position.x, -point.position.y, point.position.z+1));
//
//            glfwSwapBuffers(window);
//            glfwPollEvents();
//        };
//#ifdef __EMSCRIPTEN__
//        emscripten_set_main_loop(main_loop, 0, true);
//#else
//        while(!glfwWindowShouldClose(window))
//            main_loop();
//#endif
//    }
//
//    //GLAllErrors();
//
//    glfwTerminate();
//    return 0;
//}