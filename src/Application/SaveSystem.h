#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;


namespace Application {
    namespace SaveSystem {
        void fillTextures(const json& jTextures);
        json storeTextures();
        void fillMeshes(const json& jMeshes);
        json storeMeshes();
        void fillShaders(const json& jShaders);
        json storeShaders();
        void fillObjects(const json& jObjects);
        json storeObjects();
        void fillLights(const json& jLights);
        json storeLights();
        void fillAudioBuffers(const json& jAudioBufferss);
        json storeAudioBuffers();
        void fillScripts(const json& jScripts);
        json storeScripts();


        void parseJson(std::string file);
        void saveToJson(std::string file);
    }

}