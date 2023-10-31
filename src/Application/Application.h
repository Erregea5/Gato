#pragma once

#include "pch.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "Light.h"

#include "Listener.h"
#include "Source.h"
#include "SoundBuffer.h"
#include <set>

//All the code to set things up, run things, and clean up at the end
namespace Application
{
    class Script;
    extern std::unordered_map<std::string, Graphics::Texture> textures;
    extern std::unordered_map<std::string, Graphics::Shader> shaders;
    extern std::unordered_map<std::string, Graphics::Mesh> meshes;
    extern std::unordered_map<std::string, Object> objects;
    extern std::unordered_map<std::string, Graphics::Light> lights;
    extern std::unordered_map<std::string, Graphics::Renderer> renderers;
    extern std::unordered_map<std::string, Script> scripts;
    extern std::unordered_map<std::string, Audio::SoundBuffer> audioBuffers;


    extern std::set<Graphics::Mesh*> deletedMeshes;
    extern std::set<Graphics::Shader*> deletedShaders;
    extern std::set<Graphics::Texture*> deletedTextures;

    extern Graphics::Renderer* internalRenderer;
    extern Object* screen;

    struct logBlock { std::string input, output; };
    extern std::vector<logBlock> logger;

    extern void (*userInterface)();

    extern bool release;

    bool init(int _width, int _height, std::string _saveFile, bool _release=false);
    void runApp();
    bool startGame();
    void stopGame();
    bool runGame();
    void exitApp(bool);

}