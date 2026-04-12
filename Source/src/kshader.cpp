#include "kshader.h"

namespace kemena
{
    kShader::kShader()
    {
        shaderProgram = 0;
    }

    kShader::~kShader()
    {
        if (shaderProgram)
        {
            kDriver::getCurrent()->deleteShaderProgram(shaderProgram);
            shaderProgram = 0;
        }
    }

    string kShader::readFile(const string filePath)
    {
        string content;
        std::ifstream fileStream(filePath.c_str(), std::ios::in);

        if (!fileStream.is_open())
        {
            std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
            return "";
        }

        string line = "";
        while (!fileStream.eof())
        {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }

    void kShader::loadShadersFile(const string vertexShaderPath, const string fragmentShaderPath)
    {
        const char *vertSrc = nullptr;
        const char *fragSrc = nullptr;
        string vertStr, fragStr;

        if (!vertexShaderPath.empty())
        {
            vertStr = readFile(vertexShaderPath);
            vertSrc = vertStr.c_str();
        }
        if (!fragmentShaderPath.empty())
        {
            fragStr = readFile(fragmentShaderPath);
            fragSrc = fragStr.c_str();
        }

        shaderProgram = kDriver::getCurrent()->compileShaderProgram(vertSrc, fragSrc);
    }

    void kShader::loadShadersCode(const char *vertexShaderCode, const char *fragmentShaderCode)
    {
        shaderProgram = kDriver::getCurrent()->compileShaderProgram(vertexShaderCode, fragmentShaderCode);
    }

    void kShader::use()
    {
        kDriver::getCurrent()->bindShaderProgram(shaderProgram);
    }

    void kShader::unuse()
    {
        kDriver::getCurrent()->unbindShaderProgram();
    }

    void kShader::setShaderProgram(uint32_t program)
    {
        shaderProgram = program;
    }

    uint32_t kShader::getShaderProgram()
    {
        return shaderProgram;
    }

    void kShader::setValue(string name, std::vector<mat4> value)
    {
        kDriver::getCurrent()->setUniformMat4Array(shaderProgram, name, value);
    }

    void kShader::setValue(string name, mat4 value)
    {
        kDriver::getCurrent()->setUniformMat4(shaderProgram, name, value);
    }

    void kShader::setValue(string name, vec3 value)
    {
        kDriver::getCurrent()->setUniformVec3(shaderProgram, name, value);
    }

    void kShader::setValue(string name, vec2 value)
    {
        kDriver::getCurrent()->setUniformVec2(shaderProgram, name, value);
    }

    void kShader::setValue(string name, float value)
    {
        kDriver::getCurrent()->setUniformFloat(shaderProgram, name, value);
    }

    void kShader::setValue(string name, int value)
    {
        kDriver::getCurrent()->setUniformInt(shaderProgram, name, value);
    }

    void kShader::setValue(string name, unsigned int value)
    {
        kDriver::getCurrent()->setUniformUint(shaderProgram, name, value);
    }

    void kShader::setValue(string name, bool value)
    {
        kDriver::getCurrent()->setUniformBool(shaderProgram, name, value);
    }
}
