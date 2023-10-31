#pragma once

#include <string>

class Object;
namespace Application {
    enum class ObjectType {
        None, Texture, Mesh, Shader, Object, Light, AudioSource, Script
    };
    namespace Editor {
        void setCurrent(std::string newCurrent, ObjectType type, void* obj = 0);
        void init();
        void cleanUp();

        void inspector();
        void selector();
        void resources();
        void consoleLog();
        void gameEngine();
    }

    bool isFocusedOnGame();
    void updateResourceList();
}
