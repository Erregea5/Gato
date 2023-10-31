#include "SaveSystem.h"
#include "Application.h"
#include "Scripting.h"

namespace Application {
    namespace SaveSystem {
        void fillTextures(const json& jTextures)
        {
            for (auto& t : jTextures) {
                if (t.contains("file"))
                {
                    if (t["file"].is_array())
                    {
                        std::vector<std::string> files;
                        for (auto& f : t["file"])
                            files.push_back(f);
                        textures.try_emplace(t["name"], files);
                        //textures[t["name"]] = Graphics::Texture(files);
                    }
                    else
                    {
                        std::string file = t["file"];
                        textures.try_emplace(t["name"], file);
                        //textures[t["name"]] = Graphics::Texture(file);
                    }

                }
                else
                    textures.try_emplace(t["name"], "");//todo: try_emplace for all
                    //textures[t["name"]] = Graphics::Texture();
            }
        }
        json storeTextures()
        {
            json jTextures;
            for (auto& t : textures) {
                if (deletedTextures.count(&t.second))
                    continue;
                Graphics::Texture& texture = t.second;
                json jTexture;
                if (texture.isCube)
                    for (auto& f : texture.filePaths)
                        jTexture["file"].push_back(f);
                else
                    jTexture["file"] = texture.filePaths[0];
                jTexture["name"] = t.first;
                jTextures.push_back(jTexture);
            }
            return jTextures;
        }
        void fillMeshes(const json& jMeshes)
        {
            for (auto& m : jMeshes)
            {
                int dim = 3;
                bool hasNormals = true;
                bool hasTexture = true;
                if (m.contains("dim"))
                    dim = m["dim"];
                if (m.contains("hasNormals"))
                    hasNormals = m["hasNormals"];
                if (m.contains("hasTexture"))
                    hasTexture = m["hasTexture"];

                std::string file = m["file"];
                meshes.try_emplace(m["name"], file, dim, hasNormals, hasTexture);
            }
        }
        json storeMeshes()
        {
            json jMeshes;
            
            for (auto& m : meshes) {
                if (deletedMeshes.count(&m.second))
                    continue;
                Graphics::Mesh& mesh = m.second;
                json jMesh;
                jMesh["dim"] = mesh.dim;
                jMesh["hasNormals"] = mesh.hasNormals;
                jMesh["hasTexture"] = mesh.hasTexture;
                jMesh["file"] = mesh.filePath;
                jMesh["name"] = m.first;
                jMeshes.push_back(jMesh);
            }
            return jMeshes;
        }
        void fillShaders(const json& jShaders)
        {
            for (auto& s : jShaders)
            {
                std::string file = s["file"];
                shaders.try_emplace(s["name"], file);
            }
        }
        json storeShaders()
        {
            json jShaders;
            for (auto& s : shaders)
            {
                if (deletedShaders.count(&s.second))
                    continue;
                json jShader;
                jShader["file"] = s.second.filePath;
                jShader["name"] = s.first;
                jShaders.push_back(jShader);
            }
            return jShaders;
        }
        void fillObjects(const json& jObjects)
        {
            std::unordered_map<std::string, std::vector<std::string>> parentCache;
            for (auto& o : jObjects)
            {
                std::string name = o["name"];

                Graphics::Shader* shader = 0;
                Graphics::Texture* texture = 0;
                Graphics::Mesh* mesh = 0;
                glm::vec3 pos(0);
                float* x = &pos.x;
                glm::vec3 orient(0);
                float* ox = &orient.x;
                glm::vec3 shape(0);
                float* sx = &shape.x;
                if (o.contains("shader"))
                    shader = &shaders[o["shader"]];
                if (o.contains("texture"))
                    texture = &textures[o["texture"]]; 
                if (o.contains("mesh"))
                    mesh = &meshes[o["mesh"]];
                if (o.contains("pos"))
                    for (float p : o["pos"])
                    {
                        *x = p;
                        x++;
                    }

                if (texture)
                    objects.try_emplace(name, *mesh, *shader, *texture, pos);
                else if (shader)
                    objects.try_emplace(name, *mesh, *shader, pos);
                else if(mesh)
                    objects.try_emplace(name, *mesh, shaders["defaultShader"], pos);
                auto& obj = objects[name];

                if (o.contains("orientation"))
                {
                    for (float p : o["orientation"])
                    {
                        *ox = p;
                        ox++;
                    }
                    obj.transform.orientation = orient;
                }
                if (o.contains("shape")) {
                    for (float p : o["shape"])
                    {
                        *sx = p;
                        sx++;
                    }
                    obj.transform.shape = shape;
                }

                auto childList = parentCache.find(name);
                if (childList != parentCache.end())
                {
                    for (auto& child : childList->second)
                    {
                        auto& son = objects[child];

                        obj.transform.addChild(child, son.transform);
                        son.transform.setParent(obj.transform);
                    }
                }

                if (o.contains("parent"))
                {
                    if (o["parent"] == name)
                        continue;
                    auto parent = objects.find(o["parent"]);
                    if (parent != objects.end())
                    {
                        parent->second.transform.addChild(name, obj.transform);
                        obj.transform.setParent(parent->second.transform);
                    }
                    else
                        parentCache[o["parent"]].push_back(name);
                }
                if (o.contains("audioBuffer"))
                    obj.audio.setBuffer(audioBuffers[o["audioBuffer"]]);
            }
        }
        json storeObjects()
        {
            json jObjects;
            std::unordered_map<std::string, std::string> childCache;
            for (auto& o : objects)
            {
                Object& object = o.second;
                json jObject;
                if (object.shader&&!deletedShaders.count(object.shader))
                    for (auto& shader : shaders)
                        if (&shader.second == object.shader)
                            jObject["shader"] = shader.first;
                if (object.texture&&!deletedTextures.count(object.texture))
                    for (auto& texture : textures)
                        if (&texture.second == object.texture)
                            jObject["texture"] = texture.first;
                if (object.mesh && !deletedMeshes.count(object.mesh))
                    for (auto& mesh : meshes)
                        if (&mesh.second == object.mesh)
                            jObject["mesh"] = mesh.first;

                if (childCache.find(o.first) != childCache.end())
                    jObject["parent"] = childCache[o.first];
                
                for (auto &child : object.transform.childrenNames)
                {
                    for(auto &jO:jObjects)
                        if (jO["name"] == child)
                        {
                            jO["parent"] = o.first;
                            break;
                        }
                    childCache[child]=o.first;
                }


                jObject["pos"].push_back(object.transform.position.x);
                jObject["pos"].push_back(object.transform.position.y);
                jObject["pos"].push_back(object.transform.position.z);
                
                jObject["orientation"].push_back(object.transform.orientation.x);
                jObject["orientation"].push_back(object.transform.orientation.y);
                jObject["orientation"].push_back(object.transform.orientation.z);
                
                jObject["shape"].push_back(object.transform.shape.x);
                jObject["shape"].push_back(object.transform.shape.y);
                jObject["shape"].push_back(object.transform.shape.z);

                jObject["name"] = o.first;

                for (auto& buff : audioBuffers)
                    if (object.audio.buffer == &buff.second)
                        jObject["audioBuffer"] = buff.first;
                
                
                jObjects.push_back(jObject);
            }
            return jObjects;
        }
        void fillLights(const json& jLights)
        {
            for (auto& l : jLights)
            {
                float intensity;
                if (l.contains("intensity"))
                    intensity = l["intensity"];
                else
                    intensity = 1.0f;
                Graphics::Light::LightType type = l["type"] == "point" ? Graphics::Light::LightType::Point : Graphics::Light::LightType::Direct;
                Graphics::Shader& depthShader = shaders[l["shader"]];

                glm::vec3 position(0), orientation(1), color(1);
                float* x1 = &position.x, * x2 = &orientation.x, * x3 = &color.r;
                if (l.contains("pos"))
                    for (float p : l["pos"])
                    {
                        *x1 = p;
                        x1++;
                    }
                if (l.contains("orientation"))
                    for (float p : l["orientation"])
                    {
                        *x2 = p;
                        x2++;
                    }
                if (l.contains("color"))
                    for (float p : l["color"])
                    {
                        *x3 = p;
                        x3++;
                    }
                lights.try_emplace(l["name"], position, orientation, color, intensity, type, depthShader);
            }
        }
        json storeLights()
        {
            json jLights;
            for (auto& l : lights)
            {
                Graphics::Light& light = l.second;
                json jLight;
                jLight["intensity"] = light.intensity;

                jLight["pos"].push_back(light.getTransform().position.x);
                jLight["pos"].push_back(light.getTransform().position.y);
                jLight["pos"].push_back(light.getTransform().position.z);

                jLight["orientation"].push_back(light.getTransform().orientation.x);
                jLight["orientation"].push_back(light.getTransform().orientation.y);
                jLight["orientation"].push_back(light.getTransform().orientation.z);

                jLight["color"].push_back(light.color.x);
                jLight["color"].push_back(light.color.y);
                jLight["color"].push_back(light.color.z);

                jLight["type"] = light.type == Graphics::Light::LightType::Point ? "point" : "direct";

                for (auto& shader : shaders)
                    if (&shader.second == light.depthShader)
                        jLight["shader"] = shader.first;

                jLight["name"] = l.first;
                jLights.push_back(jLight);
            }
            return jLights;
        }
        void fillAudioBuffers(const json& jAudioBuffers)
        {
            for (auto& a : jAudioBuffers)
                audioBuffers.try_emplace(a["name"],a["file"]);     
            
        }
        json storeAudioBuffers()
        {
            json jSources;
            for (auto& a : audioBuffers)
            {
                json jSource;
                jSource["name"] = a.first;
                jSource["file"] = a.second.filePath;
                jSources.push_back(jSource);
            }
            return jSources;
        }
        void fillScripts(const json& jScripts)
        {
            for (auto& s : jScripts)
            {
                if (s.contains("file"))
                    scripts.try_emplace(s["name"],s["file"]);
            }
        }
        json storeScripts()
        {
            json jScripts;
            for (auto& s : scripts)
            {
                json jScript;
                jScript["name"] = s.first;
                jScript["file"] = s.second.filePath;
                jScripts.push_back(jScript);
            }
            return jScripts;
        }


        void parseJson(std::string file)
        {
            json buffer;
            std::ifstream input(file);
            if (input.is_open())
            {
                input >> buffer;
                fillTextures(buffer["textures"]);
                fillMeshes(buffer["meshes"]);
                fillShaders(buffer["shaders"]);
                fillAudioBuffers(buffer["audioBuffers"]);
                fillObjects(buffer["objects"]);
                fillLights(buffer["lights"]);
                fillScripts(buffer["scripts"]);
            }
            input.close();
        }
        void saveToJson(std::string file)
        {
            json buffer;
            std::ofstream output(file);

            buffer["textures"] = storeTextures();
            buffer["meshes"] = storeMeshes();
            buffer["shaders"] = storeShaders();
            buffer["objects"] = storeObjects();
            buffer["lights"] = storeLights();
            buffer["audioBuffers"] = storeAudioBuffers();
            buffer["scripts"] = storeScripts();

            output << buffer;
            output.close();
        }
    }
}