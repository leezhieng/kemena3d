#ifndef KSHADER_H
#define KSHADER_H

#include "kexport.h"
#include "kdriver.h"
#include "kdatatype.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

namespace kemena
{
    class KEMENA3D_API kShader
    {
    public:
        kShader();
        virtual ~kShader();

        string readFile(const string filePath);
        void loadShadersFile(const string vertexShaderPath, const string fragmentShaderPath);
        void loadShadersCode(const char *vertexShaderCode, const char *fragmentShaderCode);
        void use();
        void unuse();

        void setShaderProgram(uint32_t program);
        uint32_t getShaderProgram();

        void setValue(string name, std::vector<mat4> value);
        void setValue(string name, mat4 value);
        void setValue(string name, vec3 value);
        void setValue(string name, vec2 value);
        void setValue(string name, float value);
        void setValue(string name, int value);
        void setValue(string name, unsigned int value);
        void setValue(string name, bool value);

    protected:
    private:
        uint32_t shaderProgram = 0;
    };
}

#endif // KSHADER_H
