#include "Application.h"
#include "SaveSystem.h"
#include "EditorGUI.h"
#include "Scripting.h"

namespace Application
{
    std::unordered_map<std::string, Graphics::Texture> textures;
    std::unordered_map<std::string, Graphics::Shader> shaders;
    std::unordered_map<std::string, Graphics::Mesh> meshes;
    std::unordered_map<std::string, Object> objects;
    std::unordered_map<std::string, Graphics::Light> lights;
    std::unordered_map<std::string, Graphics::Renderer> renderers;
    std::unordered_map<std::string, Script> scripts;
    std::unordered_map<std::string, Audio::SoundBuffer> audioBuffers;

    std::set<Graphics::Mesh*> deletedMeshes;
    std::set<Graphics::Shader*> deletedShaders;
    std::set<Graphics::Texture*> deletedTextures;
    
    Graphics::Renderer* internalRenderer;
    Object* screen;

    std::vector<logBlock> logger;

    void (*userInterface)();

    bool release;
    
    namespace {
        Graphics::Camera* mainCamera;//internal renderers camera
        Graphics::Mesh* square;
        Graphics::Shader* screenShader;
        Graphics::Texture* color;
        Graphics::Texture* depth;
        Graphics::FrameBuffer* PostProcessingFBO;
        Graphics::Renderer* screenRenderer;
        std::string saveFile;

        bool focusedOnGame;

        void startRender()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
        void endRender()
        {
            internalRenderer->draw(objects);

            screenRenderer->clear();
            screenRenderer->draw(*screen);

            mainCamera->getInput(Graphics::window, Graphics::width, Graphics::height, isFocusedOnGame());
            Audio::Listener::setPosition(mainCamera->transform.position);
            Audio::Listener::setOrientation(mainCamera->transform.orientation);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(Graphics::window);
            glfwPollEvents();
        }
    }

    bool init(int _width, int _height, std::string _saveFile, bool _release) {
        if (!Graphics::init(_width, _height, "Gato"))
            return 0;
        if (!Audio::init())
            return 0;
        release = _release;
        
        Editor::init();
        
        square = new Graphics::Mesh(Graphics::Mesh::PreMade::Square);
        screenShader = new Graphics::Shader("res/shaders/screen.shader");
        color = new Graphics::Texture;
        depth = new Graphics::Texture;
        PostProcessingFBO = new Graphics::FrameBuffer(*color, *depth, Graphics::width, Graphics::height);
        screen = new Object(*square, *screenShader, *color, glm::vec3(0));
        screenRenderer = new Graphics::Renderer(Graphics::width, Graphics::height);
        internalRenderer = new Graphics::Renderer(*PostProcessingFBO);
        mainCamera = &(internalRenderer->camera);

        if (!release)
            userInterface = Editor::gameEngine;
        else
            userInterface = []() {};
        saveFile = _saveFile;

        initLua();
        SaveSystem::parseJson(saveFile);
        for (auto& s : scripts)
            s.second.load();

        return 1;
    }
    void runApp() {
        if (release)
        {
            runGame();
            exitApp(0);
            return;
        }
        auto gameStarter = [&]()
        {
            ImGui::SetNextWindowSize(ImVec2(170, 70));
            ImGui::SetNextWindowPos(ImVec2(Graphics::width/2-150, 0));

            if (!ImGui::Begin("Game Starter"))
            {
                ImGui::End();
                return true;
            }
            if (ImGui::Button("start game"))
            {
                ImGui::End();
                endRender();

                if (startGame())
                {
                    exitApp(1);
                    return false;
                }
                else
                    Editor::setCurrent("", ObjectType::None);
            }
            else
                ImGui::End();
            return true;
            
        };

        while (!glfwWindowShouldClose(Graphics::window))
        {
            startRender();

            
            if (!gameStarter())
                return;
            userInterface();
            internalRenderer->clear();
            endRender();
        }
        exitApp(0);
    }
    
    bool startGame()
    {
        SaveSystem::saveToJson(saveFile);
        for (auto& s : scripts) {
            s.second.load();
            s.second.runStart();
        }
        
        return runGame();
    }
    void stopGame()
    {
        textures.clear();
        shaders.clear();
        meshes.clear();
        objects.clear();
        lights.clear();
        SaveSystem::parseJson(saveFile);
        updateResourceList();
    }
    bool runGame()
    {
        bool usingInterface = 1;
        bool usingLights = 1;
        std::function<bool()> gameController;
        if (!release)
            gameController = [&](){
                ImGui::SetNextWindowPos(ImVec2(700, 0));
                ImGui::SetNextWindowSize(ImVec2(450, 70));

                if (ImGui::Begin("Game Controller"))
                {
                    if (ImGui::Button("stop game"))
                    {
                        stopGame();
                        ImGui::End();
                        endRender();
                        startRender();
                        return false;
                    }

                    ImGui::SameLine();
                    if (usingInterface)
                    {
                        userInterface();
                        if (ImGui::Button("Turn Off Interface"))
                            usingInterface = 0;
                    }
                    else if (ImGui::Button("Turn On Interface"))
                        usingInterface = 1;
                }
                ImGui::End();
                return true;
            };
        else
            gameController = []() {return true;};

        while (!glfwWindowShouldClose(Graphics::window))
        {
            startRender();

            if (!gameController())
                return false;
            
            internalRenderer->clear();
            for (auto& s : scripts)
                s.second.runUpdate();

            endRender();
        }
        return true;
    }
    void exitApp(bool inGame)
    {
        if(!inGame)
            SaveSystem::saveToJson(saveFile);
        delete lua;

        textures.clear();
        shaders.clear();
        meshes.clear();
        objects.clear();
        lights.clear();
        audioBuffers.clear();

        delete square;
        delete screenShader;
        delete color;
        delete depth;
        delete PostProcessingFBO;
        delete screen;
        delete screenRenderer;
        delete internalRenderer;

        Audio::cleanUp();
        Editor::cleanUp();
        Graphics::cleanUp();
    }

}
