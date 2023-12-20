#include "Shader.h"
#include "Renderer.h"
#include <fstream>
#include <iostream>

namespace Graphics {
    Shader::Shader(std::string _filePath)
        :textureCount(0),filePath(_filePath)
    {compile();}

    Shader::Shader()
        :textureCount(0),ID(0)
    {}

    Shader::Shader(const Shader& rhs)
        : textureCount(0),filePath(rhs.filePath)
    {compile();}

    Shader& Shader::operator=(const Shader& rhs)
    {
        textureCount = (0);
        filePath = (rhs.filePath);
        compile();
        return *this;
    }

    Shader::~Shader()
    {
        std::cout << "deleted shader " << filePath << std::endl;
        if(ID!=0)
            GLCall(glDeleteProgram(ID));
    }

    void Shader::compile()
    {
        ShaderSource source = loadShaders();
        ID = createShaderProgram(source);
    }

    void Shader::bind() const
    {
        GLCall(glUseProgram(ID));
    }

    void Shader::unBind() const
    {
        GLCall(glUseProgram(0));
    }

    void Shader::setInteger(std::string name, int v0)
    {
        GLCall(glUniform1i(getUniformLocation(name), v0));
    }

    void Shader::setFloat(std::string name, float v0)
    {
        GLCall(glUniform1f(getUniformLocation(name), v0));
    }

    void Shader::setFloat(std::string name, float v0, float v1, float v2)
    {
        GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
    }

    void Shader::setFloat(std::string name, float v0, float v1, float v2, float v3)
    {
        GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
    }

    void Shader::setMatrix(std::string name, const glm::mat4x4& v0, int n)
    {
        GLCall(glUniformMatrix4fv(getUniformLocation(name), n, GL_FALSE, &v0[0][0]));
    }

    void Shader::setLight(const Light& o)
    {
        if (o.type == Light::LightType::Point)
        {
            setFloat("u_pointLightPosition", o.getTransform().position.x, o.getTransform().position.y, o.getTransform().position.z);
            setFloat("u_pointLightColor", o.color.x, o.color.y, o.color.z);
            setFloat("u_pointLightIntensity", o.intensity);
            setFloat("u_farPlane", o.renderer->farPlane);
            setTexture("u_pointLightShadowMap", *o.texture);
            setInteger("u_usingPoint", 1);
        }
        else
        {
            setFloat("u_directLightDirection", o.getTransform().orientation.x, o.getTransform().orientation.y, o.getTransform().orientation.z);
            setFloat("u_directLightColor", o.color.x, o.color.y, o.color.z);
            setFloat("u_directLightIntensity", o.intensity);
            setTexture("u_directLightShadowMap", *o.texture);
            setMatrix("u_directLightMatrix", o.directLightView);
            setInteger("u_usingDirectional", 1);
        }
    }
    //add updateLight()

    void Shader::setTexture(std::string name, const Texture& texture)
    {
        int slot;
        if (textureCache.find(name) != textureCache.end())
            slot = textureCache[name];
        else
        {
            slot = textureCount;
            textureCache[name] = textureCount++;
        }
        texture.bind(slot);
        setInteger(name, slot);
    }

    int Shader::getUniformLocation(std::string& name)
    {
        if (uniformLocationCache.find(name) != uniformLocationCache.end())
            return uniformLocationCache[name];
        int location;
        GLCall(location = glGetUniformLocation(ID, name.c_str()));
        if (location == -1)
            std::cout << "Warning! Uniform " << name << " doesn't exist\n";
        uniformLocationCache[name] = location;
        return location;
    }

    std::vector<Shader> Shader::createShaders(int n, std::string filePath)
    {
        std::vector<Shader> out(n);
        GLuint vertexShader, fragmentShader, geometryShader;
        out[0].filePath = filePath;
        ShaderSource source = out[0].loadShaders();
        vertexShader = out[0].compileShader(GL_VERTEX_SHADER, source.VertexShader);
        fragmentShader = out[0].compileShader(GL_FRAGMENT_SHADER, source.FragmentShader);
        if (source.GeometryShader != "")
            geometryShader = out[0].compileShader(GL_GEOMETRY_SHADER, source.GeometryShader);
        for (int i = 0;i < n;i++)
        {
            out[i].filePath = filePath;
            GLuint program;
            GLCall(program = glCreateProgram());

            GLCall(glAttachShader(program, vertexShader));
            GLCall(glAttachShader(program, fragmentShader));
            if (source.GeometryShader != "")
                GLCall(glAttachShader(program, geometryShader));
            GLCall(glLinkProgram(program));
            GLCall(glValidateProgram(program));

            out[i].ID = program;
        }
        GLCall(glDeleteShader(vertexShader));
        GLCall(glDeleteShader(fragmentShader));
        if (source.GeometryShader != "")
            GLCall(glDeleteShader(geometryShader));
        return out;
    }

    std::string startingWord(const std::string& str)
    {
        int i = 0, u = 0;
        while (str[i] == ' ' && str.length() > i)
            i++;
        while (str[i + u] != ' ' && str.length() > i + u)
            u++;
        return str.substr(i, u);
    }

    ShaderSource Shader::loadShaders()
    {
        std::ifstream in(filePath);
        if (in.is_open())
            std::cout << "opened shader file: " << filePath << std::endl;
        std::string line, shaders[3];
        enum ShaderType
        {
            NoShader = -1, VertexShader = 0, FragmentShader = 1, GeometryShader = 2
        };
        ShaderType cur = ShaderType::NoShader;
        while (getline(in, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    cur = ShaderType::VertexShader;
                else if (line.find("fragment") != std::string::npos)
                    cur = ShaderType::FragmentShader;
                else if (line.find("geometry") != std::string::npos)
                    cur = ShaderType::GeometryShader;
            }
            else if (cur != ShaderType::NoShader)
            {
#ifdef __EMSCRIPTEN__
                if (line.find("layout(") != std::string::npos)
                {
                    std::string inL = " in", outL = " out";
                    int inP = line.find(inL), outP = line.find(outL);
                    shaders[cur] += "attribute";
                    if (inP != std::string::npos)
                        shaders[cur] += line.substr(inP + inL.length());
                    else if (outP != std::string::npos)
                        shaders[cur] += line.substr(outP + outL.length());
                    shaders[cur] += "\n";
                }
                else if (startingWord(line) == "in")
                    shaders[cur] += "varying" + line.substr(line.find("in") + 2);
                else if (startingWord(line) == "out")
                    shaders[cur] += "varying" + line.substr(line.find("out") + 3);
                else
#endif
                    shaders[cur] += line + "\n";
            }

        }
        in.close();
        return { shaders[0],shaders[1],shaders[2] };
    }

    unsigned int Shader::compileShader(unsigned int type, std::string& shaderCode)
    {
        GLuint shader;
        GLCall(shader = glCreateShader(type));
        const char* code = shaderCode.c_str();
        GLCall(glShaderSource(shader, 1, &code, NULL));
        GLCall(glCompileShader(shader));

        GLint result;
        GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
        if (result != GL_TRUE)
        {
            GLint length;
            GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*)alloca(length * sizeof(char));
            GLCall(glGetShaderInfoLog(shader, length, &length, message));
            std::cout << "Shader Compile Error\n" << message << std::endl;
            GLCall(glDeleteShader(shader));
            successful = false;
            return 0;
        }
        successful = true;
        return shader;
    }

    unsigned int Shader::createShaderProgram(ShaderSource source)
    {
        GLuint program, vertexShader, fragmentShader, geometryShader = 1;
        GLCall(program = glCreateProgram());
        vertexShader = compileShader(GL_VERTEX_SHADER, source.VertexShader);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, source.FragmentShader);
        if (source.GeometryShader != "")
            geometryShader = compileShader(GL_GEOMETRY_SHADER, source.GeometryShader);

        if (!(vertexShader && fragmentShader && geometryShader))
            return 0;
        GLCall(glAttachShader(program, vertexShader));
        GLCall(glAttachShader(program, fragmentShader));
        if (source.GeometryShader != "")
            GLCall(glAttachShader(program, geometryShader));
        GLCall(glLinkProgram(program));
        GLCall(glValidateProgram(program));

        GLCall(glDeleteShader(vertexShader));
        GLCall(glDeleteShader(fragmentShader));
        if (source.GeometryShader != "")
            GLCall(glDeleteShader(geometryShader));
        return program;
    }
}