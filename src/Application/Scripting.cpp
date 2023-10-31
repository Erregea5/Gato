#include "Scripting.h"
#include "Application.h"
#include "sol2_ImGui_Bindings/sol_ImGui.h"
#define MAX_VECTOR_OUTPUT 50
namespace Application {
    sol::state* lua=0;
    namespace {
        void registerMesh()
        {
            lua->new_enum<false>("PreMadeMesh",
               "Capsule", Graphics::Mesh::PreMade::Capsule,
               "Cone", Graphics::Mesh::PreMade::Cone,
               "Cube", Graphics::Mesh::PreMade::Cube,
               "Cylinder", Graphics::Mesh::PreMade::Cylinder,
               "Disk", Graphics::Mesh::PreMade::Disk,
               "Icosahedron", Graphics::Mesh::PreMade::Icosahedron,
               "Quartz", Graphics::Mesh::PreMade::Quartz,
               "Sphere", Graphics::Mesh::PreMade::Sphere,
               "Square", Graphics::Mesh::PreMade::Square
               );
            sol::usertype<Graphics::Mesh> objType = lua->new_usertype<Graphics::Mesh>("Mesh", sol::constructors<
                Graphics::Mesh(const std::string&, int, bool, bool),
                Graphics::Mesh(const std::vector<float>&, const std::vector<unsigned int>&, int, bool, bool),
                Graphics::Mesh(Graphics::Mesh::PreMade)
            >());
            objType["bind"] = &Graphics::Mesh::bind;
            objType["unBind"] = &Graphics::Mesh::unBind;
            objType["getIboCount"] = &Graphics::Mesh::getIboCount;
            
            objType["vertices"] = &Graphics::Mesh::vertices;
            objType["indices"] = &Graphics::Mesh::indices;
        }
        void registerShader()
        {
            sol::usertype<Graphics::Shader> objType = lua->new_usertype<Graphics::Shader>("Shader", sol::constructors<
                Graphics::Shader(std::string),
                Graphics::Shader()
            >());
            objType["bind"] = &Graphics::Shader::bind;
            objType["unBind"] = &Graphics::Shader::unBind;
            objType["setInteger"] = &Graphics::Shader::setInteger;
            objType["setFloat"] = sol::overload(
                sol::resolve<void(std::string, float)>(&Graphics::Shader::setFloat),
                sol::resolve<void(std::string, float, float, float)>(&Graphics::Shader::setFloat)
                );
            objType["setLight"] = &Graphics::Shader::setLight;
            objType["setMatrix"] = &Graphics::Shader::setMatrix;
            objType["setTexture"] = &Graphics::Shader::setTexture;
            objType["createShaders"] = &Graphics::Shader::createShaders;
        }
        void registerTexture()
        {
            sol::usertype<Graphics::Texture> objType = lua->new_usertype<Graphics::Texture>("Texture", sol::constructors<
                Graphics::Texture(),
                Graphics::Texture(const std::string&),
                Graphics::Texture(const std::vector<std::string>&)
            >());
            objType["bind"] = &Graphics::Texture::bind;
            objType["unBind"] = sol::resolve<void(void) const>(&Graphics::Texture::unBind);
            objType["getWidth"] = &Graphics::Texture::getWidth;
            objType["getHeight"] = &Graphics::Texture::getHeight;

        }
        void registerCamera()
        {
            sol::usertype<Graphics::Camera> objType = lua->new_usertype<Graphics::Camera>("Camera", sol::constructors<
                Graphics::Camera(glm::vec3)
            >());
            objType["transform"] = &Graphics::Camera::transform;
            objType["view"] = &Graphics::Camera::view;
            objType["controller"] = &Graphics::Camera::control;

            lua->new_enum("ControllerType",
                "None",Graphics::Controller::ControllerType::None,
                "FirstPersonFlying",Graphics::Controller::ControllerType::FirstPersonFlying);

            sol::usertype<Graphics::Controller> objType1 = lua->new_usertype<Graphics::Controller>("Controller", sol::constructors<
                Graphics::Controller(Graphics::Controller::ControllerType)
            >());
            objType1["setToNone"] = &Graphics::Controller::setToNone;
            objType1["setToFirstPersonFlying"] = &Graphics::Controller::setToFirstPersonFlying;
            objType1["sensitivity"] = &Graphics::Controller::sensitivity;
            objType1["speed"] = &Graphics::Controller::speed;
        }
        void registerFrameBuffer()
        {
            sol::usertype<Graphics::FrameBuffer> objType = lua->new_usertype<Graphics::FrameBuffer>("FrameBuffer", sol::constructors <
                Graphics::FrameBuffer(Graphics::Texture&, Graphics::Texture&, int, int, bool),
                Graphics::FrameBuffer(Graphics::Texture&, int, int, bool),
                Graphics::FrameBuffer(bool,Graphics::Texture&, int, int, bool)
            >());
            objType["bind"] = &Graphics::FrameBuffer::bind;
            objType["unBind"] = &Graphics::FrameBuffer::unBind;
        }
        void registerRenderer()
        {
            sol::usertype<Graphics::Renderer> objType = lua->new_usertype<Graphics::Renderer>("Renderer", sol::constructors<
                Graphics::Renderer(Graphics::FrameBuffer&, float),
                Graphics::Renderer(int, int, float)
            >());

            objType["clear"] = sol::overload(
                sol::resolve<void()const>(&Graphics::Renderer::clear),
                sol::resolve<void(glm::vec3)const>(&Graphics::Renderer::clear)
                );
            objType["draw"] = sol::overload(
                sol::resolve<void(Object&) const>(&Graphics::Renderer::draw),
                sol::resolve<void(std::vector<Object>&) const>(&Graphics::Renderer::draw),
                sol::resolve<void(std::unordered_map<std::string, Object>&) const>(&Graphics::Renderer::draw)
            );
            objType["draw2D"] = sol::overload(
                sol::resolve<void(Object&) const>(&Graphics::Renderer::draw2D),
                sol::resolve<void(std::vector<Object>&) const>(&Graphics::Renderer::draw2D),
                sol::resolve<void(std::unordered_map<std::string, Object>&) const>(&Graphics::Renderer::draw2D)
            );

            objType["camera"] = &Graphics::Renderer::camera;
        }
        void registerTransform()
        {
            sol::usertype<Transform> objType = lua->new_usertype<Transform>("Transform", sol::constructors<
                Transform(),
                Transform(glm::vec3,glm::vec3)
            >());

            objType["translate"] = &Transform::translate;
            objType["scale"] = sol::resolve<void(glm::vec3)>(&Transform::scale);
            objType["setPosition"] = &Transform::setPosition;
            objType["rotate"] = &Transform::rotate;
            objType["setOrientation"] = &Transform::setOrientation;
            objType["setScale"] = &Transform::setScale;
            //objType["setParent"] = &Transform::setParent;
            objType["addChild"] = &Transform::addChild;
            objType["removeChild"] = sol::overload(
                sol::resolve<void(const std::string&)>(&Transform::removeChild),
                sol::resolve<void(Transform&)>(&Transform::removeChild)
            ); 
            objType["view"] = &Transform::view;

            objType["position"] = &Transform::position;
            objType["orientation"] = &Transform::orientation;
            objType["shape"] = &Transform::shape;
            objType["childrenNames"] = &Transform::childrenNames;
        }
        void registerObject()
        {
            sol::usertype<Object> objType = lua->new_usertype<Object>("Object", sol::constructors <
                Object(Graphics::Mesh&, Graphics::Shader&, Graphics::Texture&, glm::vec3),
                Object(Graphics::Mesh &, Graphics::Shader &, glm::vec3),
                Object(Graphics::Mesh &, glm::vec3),
                Object(Graphics::Mesh::PreMade, Graphics::Shader &, Graphics::Texture &, glm::vec3),
                Object(Graphics::Mesh::PreMade, Graphics::Shader &, glm::vec3),
                Object(Graphics::Mesh::PreMade, glm::vec3),
                Object(glm::vec3)
            >());
            objType["bind"] = &Object::bind;
            objType["unBind"] = &Object::unBind;
            objType["setCameraView"] = &Object::setCameraView;
            objType["setMesh"] = &Object::setMesh;
            objType["setShader"] = &Object::setShader;
            objType["setShadow"] = &Object::setShadow;
            objType["setTexture"] = &Object::setTexture;
            objType["getMeshIboCount"] = &Object::getMeshIboCount;
            objType["physicsUpdate"] = &Object::physicsUpdate;
            objType["hasShadow"] = &Object::hasShadow;
            objType["transform"] = &Object::transform;
            objType["audio"] = &Object::audio;
        }
        void registerLight()
        {
            lua->new_enum<false>("LightType",
                "Point", Graphics::Light::LightType::Point,
                "Direct", Graphics::Light::LightType::Direct
                );
            sol::usertype<Graphics::Light> objType = lua->new_usertype<Graphics::Light>("Light", sol::constructors <
                Graphics::Light(glm::vec3, glm::vec3, glm::vec3, float, Graphics::Light::LightType, Graphics::Shader&)
            >());
            objType["setColor"] = &Graphics::Light::setColor;
            objType["setDepthShader"] = &Graphics::Light::setDepthShader;
            objType["setIntensity"] = &Graphics::Light::setIntensity;
            objType["clear"] = &Graphics::Light::clear;
            objType["transform"] = sol::property(sol::resolve<Transform&()>(& Graphics::Light::getTransform), sol::resolve<Transform& ()>(&Graphics::Light::getTransform));
            objType["draw"] = sol::overload(
                sol::resolve<void(Object&)>(&Graphics::Light::draw),
                sol::resolve<void(std::vector<Object>&)>(&Graphics::Light::draw),
                sol::resolve<void(std::unordered_map<std::string, Object>&)>(&Graphics::Light::draw)
            );
        }
        void registerGLM()
        {
            sol::usertype<glm::vec3> vec3Type = lua->new_usertype<glm::vec3>("vec3", sol::constructors<
                glm::vec3(float),
                glm::vec3(float,float,float)
            >());
            vec3Type["x"] = &glm::vec3::x;
            vec3Type["y"] = &glm::vec3::y;
            vec3Type["z"] = &glm::vec3::z;
            sol::usertype<glm::vec2> vec2Type = lua->new_usertype<glm::vec2>("vec2", sol::constructors<
                glm::vec2(float),
                glm::vec2(float, float)
            >());
            vec2Type["x"] = &glm::vec2::x;
            vec2Type["y"] = &glm::vec2::y;
            sol::usertype<glm::vec4> vec4Type = lua->new_usertype<glm::vec4>("vec4", sol::constructors<
                glm::vec4(float),
                glm::vec4(float, float, float, float)
            >());
            vec4Type["x"] = &glm::vec4::x;
            vec4Type["y"] = &glm::vec4::y;
            vec4Type["z"] = &glm::vec4::z;
            vec4Type["w"] = &glm::vec4::w;
            sol::usertype<glm::mat3> mat3Type = lua->new_usertype<glm::mat3>("mat3", sol::constructors<
                glm::mat3(glm::vec3, glm::vec3, glm::vec3)
            >());
            sol::usertype<glm::mat4> mat4Type = lua->new_usertype<glm::mat4>("mat4", sol::constructors<
                glm::mat4(glm::vec4, glm::vec4, glm::vec4, glm::vec4)
            >());

            auto math=(*lua)["math"];
            
            math["mult"] = sol::overload(
                sol::resolve<glm::mat4(const glm::mat4&, const glm::mat4&)>(glm::operator*),
                sol::resolve<glm::vec4(const glm::mat4&, const glm::vec4&)>(glm::operator*),
                sol::resolve<glm::vec4(const glm::vec4&, const glm::mat4&)>(glm::operator*),
                sol::resolve<glm::vec4(const glm::vec4&, const glm::vec4&)>(glm::operator*),
                sol::resolve<glm::vec4(const glm::vec4&, const float&)>(glm::operator*),
                sol::resolve<glm::vec4(float, const glm::vec4&)>(glm::operator*),
                sol::resolve<glm::mat3(const glm::mat3&, const glm::mat3&)>(glm::operator*),
                sol::resolve<glm::vec3(const glm::mat3&, const glm::vec3&)>(glm::operator*),
                sol::resolve<glm::vec3(const glm::vec3&, const glm::mat3&)>(glm::operator*),
                sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::operator*),
                sol::resolve<glm::vec3(const glm::vec3&, float)>(glm::operator*),
                sol::resolve<glm::vec3(float, const glm::vec3&)>(glm::operator*),
                sol::resolve<glm::mat2(const glm::mat2&, const glm::mat2&)>(glm::operator*),
                sol::resolve<glm::vec2(const glm::mat2&, const glm::vec2&)>(glm::operator*),
                sol::resolve<glm::vec2(const glm::vec2&, const glm::mat2&)>(glm::operator*),
                sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(glm::operator*),
                sol::resolve<glm::vec2(const glm::vec2&, float)>(glm::operator*),
                sol::resolve<glm::vec2(float, const glm::vec2&)>(glm::operator*)
            );
            math["add"] = sol::overload(
                sol::resolve<glm::mat4(const glm::mat4&, const glm::mat4&)>(glm::operator+),
                sol::resolve<glm::vec4(const glm::vec4&, const glm::vec4&)>(glm::operator+),
                sol::resolve<glm::vec4(const glm::vec4&, const float&)>(glm::operator+),
                sol::resolve<glm::vec4(float, const glm::vec4&)>(glm::operator+),
                sol::resolve<glm::mat3(const glm::mat3&, const glm::mat3&)>(glm::operator+),
                sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::operator+),
                sol::resolve<glm::vec3(const glm::vec3&, float)>(glm::operator+),
                sol::resolve<glm::vec3(float, const glm::vec3&)>(glm::operator+),
                sol::resolve<glm::mat2(const glm::mat2&, const glm::mat2&)>(glm::operator+),
                sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(glm::operator+),
                sol::resolve<glm::vec2(const glm::vec2&, float)>(glm::operator+),
                sol::resolve<glm::vec2(float, const glm::vec2&)>(glm::operator+)
            );
            math["subtract"] = sol::overload(
                sol::resolve<glm::mat4(const glm::mat4&, const glm::mat4&)>(glm::operator-),
                sol::resolve<glm::vec4(const glm::vec4&, const glm::vec4&)>(glm::operator-),
                sol::resolve<glm::vec4(const glm::vec4&, const float&)>(glm::operator-),
                sol::resolve<glm::vec4(float, const glm::vec4&)>(glm::operator-),
                sol::resolve<glm::mat3(const glm::mat3&, const glm::mat3&)>(glm::operator-),
                sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::operator-),
                sol::resolve<glm::vec3(const glm::vec3&, float)>(glm::operator-),
                sol::resolve<glm::vec3(float, const glm::vec3&)>(glm::operator-),
                sol::resolve<glm::mat2(const glm::mat2&, const glm::mat2&)>(glm::operator-),
                sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(glm::operator-),
                sol::resolve<glm::vec2(const glm::vec2&, float)>(glm::operator-),
                sol::resolve<glm::vec2(float, const glm::vec2&)>(glm::operator-)
            );
            math["transpose"] = sol::overload(
                sol::resolve<glm::mat4(const glm::mat4&)>(glm::transpose),
                sol::resolve<glm::mat3(const glm::mat3&)>(glm::transpose)
            );
            math["inverse"] = sol::overload(
                sol::resolve<glm::mat4(const glm::mat4&)>(glm::inverse),
                sol::resolve<glm::mat3(const glm::mat3&)>(glm::inverse)
            );
            math["cross"] = sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::cross);
            math["dot"] = sol::overload(
                sol::resolve<float(const glm::vec4&, const glm::vec4&)>(glm::dot),
                sol::resolve<float(const glm::vec3&, const glm::vec3&)>(glm::dot),
                sol::resolve<float(const glm::vec2&, const glm::vec2&)>(glm::dot)
            );
            math["length"] = sol::overload(
                sol::resolve<float(const glm::vec4&)>(glm::length),
                sol::resolve<float(const glm::vec3&)>(glm::length),
                sol::resolve<float(const glm::vec2&)>(glm::length)
            );
        }
        void registerAudio()
        {
            sol::usertype<Audio::SoundBuffer> objType = lua->new_usertype<Audio::SoundBuffer>("AudioBuffer", sol::constructors<Audio::SoundBuffer(const std::string&)>());
            objType["setSound"]=&Audio::SoundBuffer::setSound;

            lua->new_usertype<Audio::Source>(
                "AudioSource", sol::constructors<Audio::Source()>(),
                "playSound", &Audio::Source::playSound,
                "setBuffer", &Audio::Source::setBuffer,
                "removeBuffer", &Audio::Source::removeBuffer,
                "setPitch", &Audio::Source::setPitch,
                "setGain", &Audio::Source::setGain,
                "setPosition", &Audio::Source::setPosition,
                "setVelocity", &Audio::Source::setVelocity,
                "setLooping", &Audio::Source::setLooping
            );
        }
    }
    
    template <class T>
    struct optional
    {
        bool successful;
        T* obj;
        optional(bool s, T& o) :successful(s), obj(&o) {}
        T& getObj() {
            return *obj;
        }
    };

    template<class T>
    void registerOptionals()
    {
        lua->new_usertype<optional<T>>(
            "optional", sol::constructors<optional<T>(bool, T&)>(),
            "successful", &optional<T>::successful,
            "obj", sol::property(&optional<T>::getObj)
            );
    }

    template<class T>
    std::ostringstream& operator<<(std::ostringstream& os, const std::vector<T>& vec)
    {
        os << "[";
        int cnt = 0;
        for (auto f : vec)
        {
            if (cnt++ > MAX_VECTOR_OUTPUT)
            {
                os << "...";
                break;
            }
            os << f << ", ";
        }
        os << "]";
        return os;
    }

    template <typename T> 
    void printLog(const T& t) {
        std::ostringstream os;
        os << t <<'\n';
        if (logger.empty())
            logger.push_back({});
        logger.back().output += os.str();
    }

    void registerGlobals() 
    {
        lua->set(
            "objects", std::ref(objects),
            "lights", std::ref(lights),
            "meshes", std::ref(meshes),
            "shaders", std::ref(shaders),
            "textures", std::ref(textures),
            "audioBuffers", std::ref(audioBuffers),
            "mainRenderer", std::ref(internalRenderer),
            "screen", std::ref(screen)
        );
        
        registerOptionals<Graphics::Shader>();
        registerOptionals<Graphics::Texture>();
        registerOptionals<Graphics::Mesh>();
        registerOptionals<Graphics::Light>();
        registerOptionals<Object>();
        (*lua)["shaders_emplace"] = [](const std::string& name, const std::string& file) {
            auto x = shaders.try_emplace(name, file);
            return optional<Graphics::Shader>{ x.second,x.first->second };
        };
        (*lua)["textures_emplace"] = sol::overload(
            [](const std::string& name, const std::string& file) {
                auto x = textures.try_emplace(name, file);
                return optional<Graphics::Texture>{ x.second, x.first->second };
            },
            [](const std::string& name, std::vector<std::string>& file) {
                auto x = textures.try_emplace(name, file);
                return optional<Graphics::Texture>{ x.second, x.first->second };
            }
        );
        (*lua)["meshes_emplace"] = sol::overload(
            [](const std::string& name, const std::string& file, int d, bool t, bool n) {
                auto x = meshes.try_emplace(name, file,d,t,n);
                return optional<Graphics::Mesh>{ x.second, x.first->second };
            },
            [](const std::string& name, const std::vector<float>& v, const std::vector<unsigned int>& i, int d, bool t, bool n) {
                auto x = meshes.try_emplace(name, v,i,d,t,n);
                return optional<Graphics::Mesh>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh::PreMade p) {
                auto x = meshes.try_emplace(name, p);
                return optional<Graphics::Mesh>{ x.second, x.first->second };
            }
        );
        (*lua)["objects_emplace"] = sol::overload(
            [](const std::string& name, Graphics::Mesh& m, Graphics::Shader& s, Graphics::Texture& t, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, s, t, v);
                return optional<Object>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh& m, Graphics::Shader& s, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, s, v);
                return optional<Object>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh& m, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, v);
                return optional<Object>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh::PreMade m, Graphics::Shader& s, Graphics::Texture& t, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, s, t, v);
                return optional<Object>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh::PreMade m, Graphics::Shader& s, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, s, v);
                return optional<Object>{ x.second, x.first->second };
            },
            [](const std::string& name, Graphics::Mesh::PreMade m, glm::vec3& v) {
                auto x = objects.try_emplace(name, m, v);
                return optional<Object>{ x.second, x.first->second };
            }
        );
        (*lua)["lights_emplace"] = [](const std::string& name,glm::vec3& p, glm::vec3& o, glm::vec3& c, float i, Graphics::Light::LightType t, Graphics::Shader& s) {
            auto x = lights.try_emplace(name,p,o,c,i,t,s);
            return optional<Graphics::Light>{ x.second, x.first->second };
        };
        
        (*lua)["print"] = sol::overload(
            [](const std::string& str) { printLog(str); },
            [](float f) { printLog(f); },
            [](std::vector<float>& f) { printLog(f); },
            [](double f) { printLog(f); },
            [](std::vector<double>& f) { printLog(f); },
            [](int f) { printLog(f); },
            [](std::vector<int>& f) { printLog(f); },
            [](Object& f) { printLog(&f); },
            [](Graphics::Light& f) { printLog(&f); },
            [](Graphics::Mesh& f) { printLog(&f); },
            [](Graphics::Texture& f) { printLog(&f); },
            [](Graphics::Shader& f) { printLog(&f); },
            [](Graphics::Renderer& f) { printLog(&f); },
            [](Graphics::FrameBuffer& f) { printLog(&f); },
            [](glm::vec4& f) { printLog("[" + std::to_string(f.x) + ", " + std::to_string(f.y) + ", " + std::to_string(f.z) + ", " + std::to_string(f.w) + "]"); },
            [](glm::vec3& f) { printLog("[" + std::to_string(f.x) + ", " + std::to_string(f.y) + ", " + std::to_string(f.z) + "]"); },
            [](glm::vec2& f) { printLog("[" + std::to_string(f.x) + ", " + std::to_string(f.y) + "]"); }
        );
        
        (*lua)["help"] = []() {
            return "this\n"
                "is\n"
                "sparta";
        };
    }

    void initLua()
    {
        if (lua)
            delete lua;
        lua = new sol::state;
        lua->open_libraries(sol::lib::base,sol::lib::math);
        registerGLM();
        registerTransform();
        registerTexture();
        registerCamera();
        registerShader();
        registerMesh();
        registerFrameBuffer();
        registerRenderer();
        registerLight();
        registerAudio();
        registerObject();
        registerGlobals();
        
        sol_ImGui::Init(*lua);
    }
    
    std::string startingScript = R"(
-- This is your Script. Take care of it
$={}

-- you have access to all game objects,
-- lights, meshes, textures, shaders and audio
-- you can even make some here, 
-- just make sure to register them

-- This is called when the game starts
$.onStart=function()
	return 0
end

-- This is called once per frame
$.onUpdate=function()
	return 1
end
	)";
    namespace {
        std::string clean(std::string& in)
        {
            std::string out;
            for (char c : in)
                if (c != '\n' && c != '\r' && c != '\t')
                    out += c;
            return out;
        }
        std::string insert(std::string& base, std::string& input)
        {
            std::string out;
            for (char c : base)
                if (c != '$')
                    out += c;
                else
                    out += input;
            return out;
        }
        std::string getName(std::string path)
        {
            std::string name;
            int idx = path.find_last_of(".lua");

            if (idx != std::string::npos)
                path = path.substr(0, idx - 3);

            int i;
            for (i = path.length() - 1;i >= 0;i--)
            {
                if (path[i] == '/' || path[i] == '\\') {
                    break;
                }
            }
            name = path.substr(i + 1);
            return name;
        }
    }
    Script::Script(std::string _filePath)
    {
        filePath = clean(_filePath);
        name = getName(filePath);
        
        std::cout<<"dirt: "<<_filePath << "path: " << filePath << " name: " << name << std::endl;
        if (!std::ifstream(filePath))
        {
            std::ofstream out(filePath);
            out << insert(startingScript, name);
            out.close();
        }
        loaded = false;
    }
    void Script::load()
    {
        try 
        {
            lua->script_file(filePath);
            loaded = true;
        }
        catch (...) 
        {
            std::cout << "[Script error on Load]" << std::endl;
            loaded = false;
        }
    }
    void Script::runStart()
    {
        if(loaded)
            try {
                (*lua)[name]["onStart"].call();
            }
            catch (...)
            {
                std::cout << "[Script Error on Start]" << std::endl;
            }
    }
    void Script::runUpdate()
    {
        if(loaded)
            try {
                (*lua)[name]["onUpdate"].call();
            }
            catch (...)
            {
                std::cout << "[Script Error on Update]" << std::endl;
            }
    }
    
}