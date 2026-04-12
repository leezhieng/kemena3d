#ifndef KSHADER_H
#define KSHADER_H

#include "kexport.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "glm/glm.hpp"
#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#include "kdatatype.h"

namespace kemena
{
    class KEMENA3D_API kShader
    {
    public:
        kShader();
        virtual ~kShader();

        string readFile(const string filePath);
        void loadShadersFile(const string vertextShaderPath, const string fragmentShaderPath);
        void loadShadersCode(const char *vertextShaderCode, const char *fragmentShaderCode);
        void use();
        void unuse();

        void setShaderProgram(GLuint program);
        GLuint getShaderProgram();

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
        GLuint shaderProgram;
    };
}

#endif // KSHADER_H
