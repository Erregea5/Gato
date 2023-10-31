#include "EditorGUI.h"
#include "Application.h"
#include "Scripting.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#define FILE_PATH_LENGTH 150
#define NAME_LENGTH 50

namespace Application {
    namespace Editor {
        bool isMouseOnInspector;
        bool isMouseOnSelector;
        bool isMouseOnResources;
        namespace {
            std::string currentName;
            
            ObjectType currentType = ObjectType::None;
            void* current;
            bool rightClicked = false;

            ImVec2 resourcesSize,
                resourcesPos,
                newResourcesSize,
                newResourcesPos,
                inspectorSize,
                inspectorPos,
                selectorSize,
                selectorPos,
                folderSize,
                menuSize,
                addObjectSize;

            Graphics::Texture* folderTexture,
                * colorScriptTexture,
                * blueScriptTexture,
                * cubeTexture,
                * audioTexture,
                * addResourceTexture;

            std::string meshNames;
            std::string shaderNames;
            std::string textureNames;
            std::string audioNames;
            std::string transformNames;
            std::vector<Graphics::Mesh*> meshList(1,0);
            std::vector<Graphics::Shader*> shaderList(1, 0);
            std::vector<Graphics::Texture*> textureList(1, 0);
            std::vector<Audio::SoundBuffer*> audioList(1, 0);
            std::vector<Transform*> transformList(1, 0);

            template<class T>
            void updateResource(bool force,std::string& names,std::vector<T*>& list,std::pair<int,int>& check, int check0, std::unordered_map<std::string,T>& map) {
                if (map.size() != check.second || check.first >= check0 || force) {
                    names = "None";
                    names += '\0';
                    list.clear();
                    list.push_back(0);
                    for (auto& obj : map)
                    {
                        names += obj.first;
                        names += '\0';
                        list.push_back(&obj.second);
                    }
                    check = { 0,map.size() };
                }
                else
                    check.first++;
            }

            template<class T>
            void addTransformList(std::unordered_map<std::string, T>& map)
            {
                for (auto& obj : map)
                {
                    transformNames += obj.first;
                    transformNames += '\0';
                    transformList.push_back(&obj.second.getTransform());
                }
            }

            void updateResourceList(bool force=false)
            {
                const int check = 3000;
                static std::pair<int,int> meshCheck = { check,meshes.size() };
                static std::pair<int, int> shaderCheck = { check,shaders.size() };
                static std::pair<int, int> textureCheck = { check,textures.size() };
                static std::pair<int, int> audioCheck = { check,audioBuffers.size() };
                static std::pair<int, int> transformCheck = { check,lights.size() + objects.size() };
                
                updateResource<Graphics::Mesh>(force, meshNames, meshList, meshCheck, check, meshes);
                updateResource<Graphics::Shader>(force, shaderNames, shaderList, shaderCheck, check, shaders);
                updateResource<Graphics::Texture>(force, textureNames, textureList, textureCheck, check, textures);
                updateResource<Audio::SoundBuffer>(force, audioNames, audioList, audioCheck, check, audioBuffers);
                
                if (lights.size()+objects.size() != transformCheck.second || transformCheck.first >= check || force) {
                    transformNames = "None";
                    transformNames += '\0';
                    transformList.clear();
                    transformList.push_back(0);
                    addTransformList<Object>(objects);
                    addTransformList<Graphics::Light>(lights);
                    transformCheck = { 0,lights.size() + objects.size() };
                }
                else
                    transformCheck.first++;
            }

            void updateMouseFocus()
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || !(isMouseOnInspector || isMouseOnSelector))
                    rightClicked = false;
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    rightClicked = true;
                
                auto mouse=ImGui::GetMousePos();
                if (mouse.x<inspectorPos.x
                    || mouse.x>inspectorPos.x + inspectorSize.x
                    || mouse.y<inspectorPos.y
                    || mouse.y>inspectorPos.y + inspectorSize.y)
                    isMouseOnInspector = false;
                else
                    isMouseOnInspector = true;

                if (mouse.x<selectorPos.x
                    || mouse.x>selectorPos.x + selectorSize.x
                    || mouse.y<selectorPos.y
                    || mouse.y>selectorPos.y + selectorSize.y)
                    isMouseOnSelector = false;
                else
                    isMouseOnSelector = true;

                if (mouse.x<resourcesPos.x
                    || mouse.x>resourcesPos.x + resourcesSize.x
                    || mouse.y<resourcesPos.y
                    || mouse.y>resourcesPos.y + resourcesSize.y)
                    isMouseOnResources = false;
                else
                    isMouseOnResources = true;
            }

            template<class T>
            int search(T* obj, std::unordered_map<std::string, T>& map) {
                int i = 1;
                for (auto& p : map) {
                    if (obj == &p.second)
                        return i;
                    i++;
                }

                return 0;
            }

            void inspectTransform(Transform& transform)
            {
                ImGui::Text("Transform");
                ImGui::Text("#############################################");
                ImGui::DragFloat3("position", &transform.position.x, .1f, -FLT_MAX, FLT_MAX);
                ImGui::DragFloat3("orientation", &transform.orientation.x, .1f, -FLT_MAX, FLT_MAX);
                ImGui::DragFloat3("scale", &transform.shape.x, .1f, -FLT_MAX, FLT_MAX);

                static Transform* prev=0;
                static int curParent;

                if (prev != &transform)
                {
                    int i = 0;
                    for (auto& p : objects) {
                        if (transform.parent == &p.second.transform)
                        {
                            curParent = i + 1;
                            break;
                        }
                        i++;
                    }
                    if (i == objects.size())
                    {
                        for (auto& p : lights) {
                            if (transform.parent == &p.second.getTransform())
                            {
                                curParent = i + 1;
                                break;
                            }
                            i++;
                        }
                    }
                    if (i == objects.size()+lights.size())
                        curParent = 0;
                }
                ImGui::Combo("Parent", &curParent, transformNames.c_str());
                
                if (transformList[curParent] != transform.parent)
                {
                    bool ancestor=false;
                    auto cur = transformList[curParent];
                    while (cur)
                    {
                        if (cur == &transform)
                        {
                            ancestor = true;
                            curParent = 0;
                            break;
                        }
                        cur = cur->parent;
                    }
                    if (!ancestor)
                    {
                        if (transform.parent != 0)
                            transform.parent->removeChild(transform);
                        if (transformList[curParent] != 0)
                            transformList[curParent]->addChild(currentName, transform);
                    }
                }
                prev = &transform;
            }
            
            template<class T>
            void selectObject(std::string name, T& obj, ObjectType type)
            {
                ImGui::PushID(&obj);

                bool node_open = ImGui::TreeNode(name.c_str());
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                    setCurrent(name, type, &obj);
                if (node_open)
                {
                    for (std::string& childName : obj.getTransform().childrenNames)
                    {
                        auto findObj = objects.find(childName);
                        if (findObj != objects.end())
                            selectObject<Object>(childName, findObj->second,ObjectType::Object);
                        else
                            selectObject<Graphics::Light>(childName, lights[childName],ObjectType::Light);
                    }

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }

        void init() 
        {
            resourcesSize = ImVec2(Graphics::width - 1, 350);
            resourcesPos = ImVec2(0, Graphics::height - resourcesSize.y);

            inspectorSize = ImVec2(430, resourcesPos.y);
            inspectorPos = ImVec2(1500, 0);

            selectorSize = ImVec2(300, resourcesPos.y);
            selectorPos = ImVec2(0, 0);

            newResourcesSize = ImVec2(300, 350);
            newResourcesPos = ImVec2(Graphics::width/2 - newResourcesSize.x, Graphics::height/2 - newResourcesSize.y);

            folderSize = ImVec2(60, 60);
            menuSize = ImVec2(60, 40);
            addObjectSize = ImVec2(15, 15);

            auto initNames = [](std::string& names)
            {
                names = "None";
                names += '\0';
            };
            initNames(meshNames);
            initNames(shaderNames);
            initNames(textureNames);
            initNames(audioNames);
            initNames(transformNames);

            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(Graphics::window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF("res\\fonts\\Cousine-Regular.ttf",18);

            folderTexture = new Graphics::Texture("res/textures/folderImage.png");
            colorScriptTexture = new Graphics::Texture("res/textures/colorImage.png");
            blueScriptTexture = new Graphics::Texture("res/textures/luaImage.png");
            cubeTexture = new Graphics::Texture("res/textures/cubeImage.png");
            audioTexture = new Graphics::Texture("res/textures/audioSourceImage.png");
            addResourceTexture = new Graphics::Texture("res/textures/addImage.png");
        }
        void cleanUp()
        {
            delete folderTexture;
            delete colorScriptTexture;
            delete blueScriptTexture;
            delete cubeTexture;
            delete audioTexture;
            delete addResourceTexture;

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void setCurrent(std::string newCurrent, ObjectType type, void* obj)
        {
            currentName = newCurrent;
            currentType = type;
            if (type == ObjectType::None)
                current = 0;
            else if (obj)
                current = obj;
            else
                switch (type) {
                case ObjectType::Object:
                    current = (void*)&Application::objects[currentName];
                    break;
                case ObjectType::Light:
                    current = (void*)&Application::lights[currentName];
                    break;
                case ObjectType::Script:
                    current = (void*)&Application::scripts[currentName];
                    break;
                case ObjectType::Mesh:
                    current = (void*)&Application::meshes[currentName];
                    break;
                case ObjectType::Shader:
                    current = (void*)&Application::shaders[currentName];
                    break;
                case ObjectType::Texture:
                    current = (void*)&Application::textures[currentName];
                    break;
                case ObjectType::AudioSource:
                    current = (void*)&Application::audioBuffers[currentName];
                    break;
                }
        }

        template <class T>
        void deleteCurrent(T* currentObject,std::unordered_map<std::string,T>& map,std::set<T*>* set=0)
        {
            if (ImGui::Button("Delete"))
            {
                if (set)
                    set->insert(currentObject);
                else
                    for (auto& p : map)
                        if (&p.second == currentObject)
                        {
                            map.erase(p.first);
                            setCurrent("", ObjectType::None, 0);
                            break;
                        } 
            }
        }

        void inspector()
        {
#ifdef FIXED_POSITION
            ImGui::SetNextWindowSize(inspectorSize);
            ImGui::SetNextWindowPos(inspectorPos);
#endif
            if (!ImGui::Begin("Inspector"))
            {
                isMouseOnInspector = false;
                ImGui::End();
                return;
            }

            if (currentType != ObjectType::None)
            {
                ImGui::Text(("# " + currentName).c_str());
                ImGui::Text("#############################################");
            }
            switch (currentType) 
            {
            case ObjectType::Texture:
            {
                auto currentTexture = (Graphics::Texture*)current;
                deleteCurrent<Graphics::Texture>(currentTexture, textures, &deletedTextures);
                break;
            }
            case ObjectType::Mesh:
            {
                auto currentMesh = (Graphics::Mesh*)current;
                deleteCurrent<Graphics::Mesh>(currentMesh, meshes, &deletedMeshes);
                break;
            }
            case ObjectType::Shader:
            {
                auto currentShader = (Graphics::Shader*)current;
                if (ImGui::Button("Recompile"))
                    currentShader->compile();
                if (ImGui::Button("Open in VSCode"))
                {
                    auto cmd = "code " + currentShader->filePath;
                    system(cmd.c_str()); 
                }

                deleteCurrent<Graphics::Shader>(currentShader, shaders, &deletedShaders);
                break;
            }
            case ObjectType::Script:
            {
                auto currentScript = (Script*)current;
                if (ImGui::Button("Open in VSCode"))
                {
                    auto cmd = "code " + currentScript->filePath;
                    system(cmd.c_str());
                }

                deleteCurrent<Script>(currentScript, scripts);
                break;
            }
            case ObjectType::Object:
            {
                auto currentObject = (Object*)current;
                static Object* prevObject=0;
                static int curMesh;
                static int curShader;
                static int curTexture;
                static int curAudio;
                    
                if (currentObject != prevObject)
                {
                    curMesh = search(currentObject->mesh, meshes);
                    curShader = search(currentObject->shader, shaders);
                    curTexture = search(currentObject->texture, textures);
                    curAudio = search(currentObject->audio.buffer, audioBuffers);
                }
                ImGui::Checkbox("is Active", &currentObject->isActive);
                inspectTransform(currentObject->transform);
                ImGui::Text("Properties");
                ImGui::Text("#############################################");
                ImGui::Combo("Mesh", &curMesh, meshNames.c_str());
                ImGui::Combo("Shader", &curShader, shaderNames.c_str());
                ImGui::Combo("Texture", &curTexture, textureNames.c_str());
                ImGui::Combo("Audio Buffer", &curAudio, audioNames.c_str());
                if(ImGui::Button("Update Object"))
                {
                    if (!curMesh)
                        currentObject->removeMesh();
                    else
                        currentObject->setMesh(*meshList[curMesh]);
                    if (!curShader)
                        currentObject->removeShader();
                    else
                        currentObject->setShader(*shaderList[curShader]);
                    if (!curTexture)
                        currentObject->removeTexture();
                    else
                        currentObject->setTexture(*textureList[curTexture]);
                    if (!curAudio)
                        currentObject->audio.removeBuffer();
                    else
                        currentObject->audio.setBuffer(*audioList[curAudio]);
                }
                if (currentObject->audio.buffer)
                    if (ImGui::Button("Play Sound"))
                        currentObject->audio.playSound();
                prevObject = currentObject;
                
                deleteCurrent<Object>(currentObject, objects);
                break;
            }
            case ObjectType::Light:
            {
                auto currentLight = (Graphics::Light*)current;
                ImGui::Checkbox("is Active", &currentLight->isActive);
                inspectTransform(currentLight->renderer->camera.transform);
                ImGui::Text("Properties");
                ImGui::DragFloat("Light Intensity", &currentLight->intensity, .1f, -FLT_MAX, FLT_MAX);
                ImGui::ColorEdit3("Light Color", &currentLight->color.r);

                deleteCurrent<Graphics::Light>(currentLight, lights);
                break;
            }
            }

            ImGui::End();
        }

        bool addObject() 
        {
            bool p_open = 1;
#ifdef FIXED_POSITION
            ImGui::SetNextWindowSize(newResourcesSize);
            ImGui::SetNextWindowPos(newResourcesPos);
#endif
            static char name[NAME_LENGTH] = "";
            if (!ImGui::Begin("New Object", &p_open))
            {
                ImGui::End();
                return 1;
            }
            if (ImGui::Button("Cancel") || !p_open)
            {
                ImGui::End();
                memset(name, 0, NAME_LENGTH);
                return 0;
            }

            static int type;
            const char* types[] = { "object","light" };
            ImGui::Combo("Type of Object", &type, types, 2);

            ImGui::InputTextWithHint("object name", "enter name", name, NAME_LENGTH);
            switch (type)
            {
            case 0:
            {
                static int curMesh=0;
                static int curShader=0;
                static int curTexture=0;
                static int curAudio=0;
                ImGui::Combo("Mesh", &curMesh, meshNames.c_str());
                ImGui::Combo("Shader", &curShader, shaderNames.c_str());
                ImGui::Combo("Texture", &curTexture, textureNames.c_str());
                ImGui::Combo("Audio Buffer", &curAudio, audioNames.c_str());
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    std::pair<std::unordered_map<std::string, Object>::iterator, bool> cond;
                    if (curShader)
                    {
                        if (curTexture)
                            cond = objects.try_emplace(name, *meshList[curMesh], *shaderList[curShader], *textureList[curTexture]);
                        else
                            cond = objects.try_emplace(name, *meshList[curMesh], *shaderList[curShader]);
                    }
                    else if (curMesh)
                    {
                        cond = objects.try_emplace(name, *meshList[curMesh]);
                        if (curTexture)
                            cond.first->second.setTexture(*textureList[curTexture]);
                    }
                    else
                    {
                        cond = objects.try_emplace(name);
                        if (curTexture)
                            cond.first->second.setTexture(*textureList[curTexture]);
                        if (curShader)
                            cond.first->second.setShader(*shaderList[curShader]);
                    }
                    if (curAudio)
                        cond.first->second.audio.setBuffer(*audioList[curAudio]);
                    ImGui::End();
                    memset(name, 0, NAME_LENGTH);
                    return 0;
                }
                break;
            }
            case 1:
            {
                static glm::vec3 color;
                ImGui::ColorEdit3("light color", &color.x);

                static int lightType;
                const char* types[2] = { "Direct","Point" };
                ImGui::Combo("Type of Light", &lightType, types,2);
                static int curShader;
                ImGui::Combo("Depth Shader", &curShader, shaderNames.substr(5).c_str());
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    lights.try_emplace(name, glm::vec3{ 0,0,0 }, glm::vec3{ 0,0,1 }, color, 1.f, (Graphics::Light::LightType)lightType,*shaderList[curShader]);
                    ImGui::End();
                    memset(name, 0, NAME_LENGTH);
                    return 0;
                }
                break;
            }
            }

            ImGui::End();
            return 1;
        }

        void selector()
        {
            static bool newObject=0;

            if (newObject)
                newObject = addObject();
#ifdef FIXED_POSITION
            ImGui::SetNextWindowSize(selectorSize);
            ImGui::SetNextWindowPos(selectorPos);
#endif
            if (!ImGui::Begin("Object Selector"))
            {
                isMouseOnSelector = false;
                ImGui::End();
                return;
            }
            
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            
            for (auto& obj : objects)
                if (!obj.second.transform.parent)
                    selectObject<Object>(obj.first, obj.second,ObjectType::Object);
            for (auto& obj : lights)
                if (!obj.second.getTransform().parent)
                    selectObject<Graphics::Light>(obj.first, obj.second,ObjectType::Light);


            if (ImGui::ImageButton("add object", (void*)addResourceTexture->ID, addObjectSize, { 0,1 }, { 1,0 }))
                newObject = 1;

            ImGui::PopStyleVar();
            ImGui::End();
        }
        
        void InfosPane(const char* vFilter, IGFDUserDatas vUserDatas, bool* vCantContinue)
        {
            static char newFileBuffer[NAME_LENGTH]="";
            if (ImGui::InputText("Create New File", newFileBuffer, NAME_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                std::ofstream{ ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" + newFileBuffer};
                memset(newFileBuffer, 0, NAME_LENGTH);
            }
        }

        void openFileDialog(char buffer[FILE_PATH_LENGTH], const std::string& filters)
        {
            if (ImGui::Button("Select File"))
                ImGuiFileDialog::Instance()->OpenDialogWithPane("ChooseFileDlgKey", "Choose File", filters.c_str(), ".","",InfosPane);
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                    strcpy_s(buffer, FILE_PATH_LENGTH, ImGuiFileDialog::Instance()->GetFilePathName().c_str());

                ImGuiFileDialog::Instance()->Close();
            }
        }

        template<class T>
        void singleAdd(std::pair<const std::string,T>& res,ObjectType type,char* name,char* filePath, std::set<T*>* deletedSet=0)
        {
            if(deletedSet)
                deletedSet->erase(&res.second);
            setCurrent(res.first, type, &res.second);
            ImGui::End();
            memset(name, 0, NAME_LENGTH);
            memset(filePath, 0, FILE_PATH_LENGTH);
        }

        bool addResource(int selected) {
            bool p_open=1;
#ifdef FIXED_POSITION
            ImGui::SetNextWindowSize(newResourcesSize);
            ImGui::SetNextWindowPos(newResourcesPos);
#endif
            static char name[NAME_LENGTH] = "";
            static char filePath[FILE_PATH_LENGTH] = "";
            if (!ImGui::Begin("New Resource", &p_open))
            {
                ImGui::End();
                return 1;
            }
            ImGui::Text("");
            ImGui::SameLine((newResourcesSize.x - 100) / 2);
            if (ImGui::Button("Cancel", { 100,0 }) || !p_open || !selected)
            {
                ImGui::End();
                memset(name, 0, NAME_LENGTH);
                memset(filePath, 0, FILE_PATH_LENGTH);
                return 0;
            }
            ImGui::InputTextWithHint("Name", "Input Name", name, NAME_LENGTH);
            
            switch (selected)
            {
            case 1:
            {
                ImGui::Text("# Texture");
                ImGui::Text("#############################################");
                openFileDialog(filePath, "Image Files{.jpg,.jpeg,.png,.gif}");
                ImGui::Text(filePath);
                if (ImGui::Button("Create Texture") && name[0] != '\0')
                {
                    auto iter=textures.try_emplace(std::string(name), std::string(filePath));
                    singleAdd(*iter.first, ObjectType::Texture, name, filePath, &deletedTextures);
                    return 0;
                }
                static std::vector<char[FILE_PATH_LENGTH]> filePaths(6);
                const char* faces[6] = {"Left", "Right", "Down", "Up", "Front", "Back"};
                ImGui::Text("# Cube Texture");
                ImGui::Text("#############################################");
                for (char i = 0;i < 6;i++)
                {
                    openFileDialog(filePaths[i], "Image Files{.jpg,.jpeg,.png,.gif}");
                    ImGui::SameLine();
                    ImGui::Text(filePaths[i]);
                }
                if (ImGui::Button("Create Cube Texture") && name[0] != '\0')
                {
                    std::vector<std::string> paths(6);
                    for (char i = 0;i < 6;i++)
                        paths[i] = std::string(filePaths[i]);
                    auto iter=textures.try_emplace(std::string(name), paths);
                    singleAdd(*iter.first, ObjectType::Texture, name, filePath, &deletedTextures);
                    return 0;
                }
                break;
            }
            case 2:
            {
                static int premade;
                const static char* premadeList[9] = {"Cube", "Cone", "Capsule", "Cylinder", "Sphere", "Icosahedron", "Quartz", "Disk", "Square"};
                ImGui::Text("# Use a Pre-Made Mesh:");
                ImGui::Text("#############################################");
                ImGui::Combo("Pre-Made Meshes", &premade, premadeList, 9);
                if (ImGui::Button("Create Pre-Made") && name[0] != '\0')
                {
                    auto iter=meshes.try_emplace(std::string(name), (Graphics::Mesh::PreMade)premade);
                    singleAdd(*iter.first, ObjectType::Mesh, name, filePath, &deletedMeshes);
                    return 0;
                }

                ImGui::Text("# Create a New Mesh:");
                ImGui::Text("#############################################");
                openFileDialog(filePath, ".obj");
                ImGui::Text(filePath);
                static bool hasNormal=0;
                static bool hasTexture=0;
                static int dim=3;
                ImGui::InputInt("Dimensions", &dim);
                ImGui::Checkbox("Has Normals?", &hasNormal);
                ImGui::Checkbox("Has Texture?", &hasTexture);
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    auto iter=meshes.try_emplace(std::string(name), std::string(filePath), dim, hasNormal, hasTexture);
                    singleAdd(*iter.first, ObjectType::Mesh, name, filePath, &deletedMeshes);
                    return 0;
                }
                break;
            }
            case 3:
            {
                ImGui::Text("# Open Shader");
                ImGui::Text("#############################################");
                openFileDialog(filePath, "Shader Files{.shader,.glsl}");
                ImGui::Text(filePath);
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    auto iter=shaders.try_emplace(std::string(name), std::string(filePath));
                    singleAdd(*iter.first, ObjectType::Shader, name, filePath, &deletedShaders);
                    return 0;
                }
                break;
            }
            case 4:
            {
                ImGui::Text("# Open Script");
                ImGui::Text("#############################################");
                openFileDialog(filePath, ".lua");
                ImGui::Text(filePath);
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    auto iter=scripts.try_emplace(std::string(name), std::string(filePath));
                    singleAdd(*iter.first, ObjectType::Script, name, filePath);
                    return 0;
                }
                break;
            }
            case 5:
            {
                ImGui::Text("# Open Audio File");
                ImGui::Text("#############################################");
                openFileDialog(filePath, "Sound Files{.wav,.aiff}");
                ImGui::Text(filePath);
                if (ImGui::Button("Create New") && name[0] != '\0')
                {
                    auto iter = audioBuffers.try_emplace(std::string(name), std::string(filePath));
                    singleAdd(*iter.first, ObjectType::AudioSource, name, filePath);
                    return 0;
                }
                break;
            }
            }
            

            ImGui::End();
            return 1;
        }

        template<class T>
        void singleResource(bool& newResource, int& selected, std::unordered_map<std::string, T>& map, void (*func)(std::pair<const std::string, T>&), std::set<T*>* deletedSet = 0)
        {
            if (ImGui::ArrowButton("back", ImGuiDir_Left))
                selected = 0;
            int size = map.size();
            if (deletedSet)
                size -= deletedSet->size();
            if (size > 0)
                ImGui::Columns(size, 0, false);
            for (auto& res : map)
            {
                if (deletedSet && deletedSet->count(&res.second))
                    continue;
                func(std::ref(res));
                ImGui::Text(res.first.c_str());
                ImGui::NextColumn();
            }
            if (ImGui::ImageButton("add", (void*)addResourceTexture->ID, folderSize, { 0,1 }, { 1,0 }))
                newResource = 1;
        }

        void resources()
        {
            static bool newResource = 0;
            static int selected = 0;

            if (newResource)
                newResource=addResource(selected);
#ifdef FIXED_POSITION
            ImGui::SetNextWindowSize(resourcesSize);
            ImGui::SetNextWindowPos(resourcesPos);
#endif
            if (!ImGui::Begin("Resources"))
            {
                isMouseOnResources = false;
                ImGui::End();
                return;
            }

            switch (selected)
            {
            case 0:
            {
                const std::vector<std::string> arr = {"Textures","Meshes","Shaders","Scripts","Audio Buffers"};
                int i = 1;
                int size = arr.size();
                if (size > 0)
                    ImGui::Columns(size, 0, false);
                for (auto& s : arr)
                {   
                    if (ImGui::ImageButton(s.c_str(), (void*)folderTexture->ID, folderSize, {0,1}, {1,0}))
                        selected = i;
                    i++;
                    ImGui::Text(s.c_str());
                    ImGui::NextColumn();

                }
                break;
            }
            case 1:
            {
                singleResource<Graphics::Texture>(newResource, selected, textures, [](std::pair<const std::string, Graphics::Texture>& res)
                    {
                        if (res.second.isCube)
                        {
                            if (ImGui::ImageButton(res.first.c_str(), (void*)cubeTexture->ID, folderSize, { 0,1 }, { 1,0 }))
                                setCurrent(res.first, ObjectType::Texture, &res.second);
                        }
                        else if (ImGui::ImageButton(res.first.c_str(), (void*)res.second.ID, folderSize, { 0,1 }, { 1,0 }))
                            setCurrent(res.first, ObjectType::Texture, &res.second);
                    }, &deletedTextures);
                break;
            }
            case 2:
            {
                singleResource<Graphics::Mesh>(newResource, selected, meshes, [](std::pair<const std::string, Graphics::Mesh>& res)
                    {
                        if (ImGui::ImageButton(res.first.c_str(), (void*)cubeTexture->ID, folderSize, { 0,1 }, { 1,0 }))
                            setCurrent(res.first, ObjectType::Mesh, &res.second);
                    }, &deletedMeshes);
                break;
            }
            case 3:
            {
                singleResource<Graphics::Shader>(newResource, selected, shaders, [](std::pair<const std::string, Graphics::Shader>& res)
                    {
                        if (ImGui::ImageButton(res.first.c_str(), (void*)colorScriptTexture->ID, folderSize, { 0,1 }, { 1,0 }))
                            setCurrent(res.first, ObjectType::Shader, &res.second);
                    }, &deletedShaders);
                break;
            }
            case 4:
            {
                singleResource<Script>(newResource, selected, scripts, [](std::pair<const std::string, Script>& res)
                    {
                        if (ImGui::ImageButton(res.first.c_str(), (void*)blueScriptTexture->ID, folderSize, { 0,1 }, { 1,0 })) 
                            setCurrent(res.first, ObjectType::Script, &res.second);
                    });
                break;
            }
            case 5:
            {
                singleResource<Audio::SoundBuffer>(newResource, selected, audioBuffers, [](std::pair<const std::string, Audio::SoundBuffer>& res)
                    {
                        if (ImGui::ImageButton(res.first.c_str(), (void*)audioTexture->ID, folderSize, { 0,1 }, { 1,0 }))
                            setCurrent(res.first, ObjectType::AudioSource, &res.second);
                    });
                break;
            }
            }
            ImGui::End();
        }

        namespace {
            int historyIdx = 0;
        }
        int inputBufferCallBack(ImGuiInputTextCallbackData* data)
        {
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                historyIdx--;
                data->DeleteChars(0, data->BufTextLen);
                if (historyIdx < logger.size() && historyIdx>=0)
                    data->InsertChars(0, logger[historyIdx].input.c_str());
                else
                    historyIdx = -1;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                historyIdx++;
                data->DeleteChars(0, data->BufTextLen);
                if (historyIdx < logger.size() && historyIdx>=0)
                    data->InsertChars(0, logger[historyIdx].input.c_str());
                else
                    historyIdx = logger.size();
            }
            return 0;
        }

        void consoleLog()
        {
            if (!ImGui::Begin("Log"))
            {
                ImGui::End();
                return;
            }

            static char inputBuffer[200] = "";
            static std::string inputText;
            static bool continued = 0;

            ImGui::BeginChild("log text", {0,-32});

            
            if (!logger.empty())
            {
                for (int i = 0;i < logger.size() - 1;i++)
                {
                    if (!logger[i].input.empty())
                        ImGui::Text(("> " + logger[i].input).c_str());
                    if (!logger[i].output.empty())
                        ImGui::Text(logger[i].output.c_str());
                }
                if (!logger.back().input.empty())
                {
                    if (!continued)
                        ImGui::Text(("> " + logger.back().input).c_str());
                    else
                        ImGui::Text(("+ " + logger.back().input).c_str());
                }
                if (!logger.back().output.empty())
                    ImGui::Text(logger.back().output.c_str());
            }
            ImGui::EndChild();
            ImGui::Separator();

            if(ImGui::InputText("console input", inputBuffer, 200, ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackHistory|ImGuiInputTextFlags_CallbackResize,inputBufferCallBack))
            {
                if (memcmp(inputBuffer, "clear()", 7) == 0)
                {
                    logger.clear();
                    continued = false;
                }
                else
                {
                    inputText += inputBuffer;
                    
                    if (!inputText.empty() && inputText.back() == '+')
                    {
                        inputText.back() = '\n';
                        if (!continued)
                        {
                            logger.push_back({ inputText });
                            continued = true;
                        }
                        else
                            logger.back().input = inputText;
                    }
                    else
                    {
                        if (continued)
                        {
                            logger.back().input = inputText;
                            continued = false;
                        }
                        else
                            logger.push_back({ inputText });
                        
                        std::string printInput = "print(";
                        printInput += inputText;
                        printInput += ')';
                        if (!lua->do_string(printInput).valid())
                            if (!lua->do_string(inputText).valid())
                                logger.back().output = "Error\n";
                        inputText = "";
                    }
                }
                memset(inputBuffer, 0, 200);
                historyIdx = logger.size();
            }
            
            ImGui::End();
        }

        void gameEngine()
        {
            updateMouseFocus();
            updateResourceList();
            selector();
            inspector();
            resources();
            consoleLog();
        }
    }

    bool isFocusedOnGame()
    {
        return !(Editor::isMouseOnInspector||Editor::isMouseOnSelector||Editor::isMouseOnResources);
    }
    void updateResourceList()
    {
        Editor::updateResourceList(true);
    }

}