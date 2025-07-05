#ifndef KSHADER_H
#define KSHADER_H

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
    class kShader
    {
        public:
            kShader();
            virtual ~kShader();

            std::string readFile(const std::string filePath);
            void loadShadersFile(const std::string vertextShaderPath, const std::string fragmentShaderPath);
            void loadShadersCode(const char* vertextShaderCode, const char* fragmentShaderCode);
            void use();
            void unuse();

            void setShaderProgram(GLuint program);
            GLuint getShaderProgram();

            void setValue(std::string name, std::vector<glm::mat4> value);
            void setValue(std::string name, glm::mat4 value);
            void setValue(std::string name, glm::vec3 value);
            void setValue(std::string name, glm::vec2 value);
            void setValue(std::string name, float value);
            void setValue(std::string name, int value);
            void setValue(std::string name, unsigned int value);
            void setValue(std::string name, bool value);

        protected:

        private:
            GLuint shaderProgram;
    };
}

#endif // KSHADER_H
