#include "kshader.h"

namespace kemena
{
    kShader::kShader()
    {
    }

    kShader::~kShader()
    {
    }

    std::string kShader::readFile(const std::string filePath)
    {
        std::string content;
        std::ifstream fileStream(filePath.c_str(), std::ios::in);

        if(!fileStream.is_open())
        {
            std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
            return "";
        }

        std::string line = "";
        while(!fileStream.eof())
        {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }

    void kShader::loadShadersFile(const std::string vertexShaderPath, const std::string fragmentShaderPath)
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        GLint result = GL_FALSE;
        int logLength;

        // Read shaders
        if (vertexShaderPath != "")
        {
            std::string vertexShaderStr = readFile(vertexShaderPath);
            const char *vertexShaderSrc = vertexShaderStr.c_str();

            // Compile vertex shader
            //std::cout << "Compiling vertex shader." << std::endl;
            glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
            glCompileShader(vertexShader);

            // Check vertex shader
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

            if (result == GL_FALSE && logLength > 1)
            {
                std::vector<char> shaderError((logLength > 1) ? logLength : 1);
                glGetShaderInfoLog(vertexShader, logLength, nullptr, &shaderError[0]);
                std::cout << "Vertex Shader Error: " << &shaderError[0] << "(" << vertexShaderPath << ")" << std::endl;
            }
        }

        if (fragmentShaderPath != "")
        {
            std::string fragmentShaderStr = readFile(fragmentShaderPath);
            const char *fragmentShaderSrc = fragmentShaderStr.c_str();

            // Compile fragment shader
            //std::cout << "Compiling fragment shader." << std::endl;
            glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
            glCompileShader(fragmentShader);

            // Check fragment shader
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

            if (result == GL_FALSE && logLength > 1)
            {
                std::vector<char> shaderError((logLength > 1) ? logLength : 1);
                glGetShaderInfoLog(fragmentShader, logLength, nullptr, &shaderError[0]);
                std::cout << "Fragment Shader Error: " << &shaderError[0] << "(" << fragmentShaderPath << ")" << std::endl;
            }
        }

        // Program
        //std::cout << "Linking program" << std::endl;
        shaderProgram = glCreateProgram();

        if (vertexShaderPath != "")
        {
            glAttachShader(shaderProgram, vertexShader);
        }
        if (vertexShaderPath != "")
        {
            glAttachShader(shaderProgram, fragmentShader);
        }

        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

        if (result == GL_FALSE && logLength > 1)
        {
            std::vector<char> programError( (logLength > 1) ? logLength : 1 );
            glGetProgramInfoLog(shaderProgram, logLength, nullptr, &programError[0]);
            std::cout << "Link Shader Program Error: " << &programError[0] << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void kShader::loadShadersCode(const char* vertextShaderCode, const char* fragmentShaderCode)
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        GLint result = GL_FALSE;
        int logLength;

        // Read shaders
        if (vertextShaderCode && strlen(vertextShaderCode) > 0)
        {
            // Compile vertex shader
            //std::cout << "Compiling vertex shader." << std::endl;
            glShaderSource(vertexShader, 1, &vertextShaderCode, nullptr);
            glCompileShader(vertexShader);

            // Check vertex shader
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

            if (result == GL_FALSE && logLength > 1)
            {
                std::vector<char> shaderError((logLength > 1) ? logLength : 1);
                glGetShaderInfoLog(vertexShader, logLength, nullptr, &shaderError[0]);
                std::cout << "Vertex Shader Error: " << &shaderError[0] << "(" << vertextShaderCode << ")" << std::endl;
            }
        }

        if (fragmentShaderCode && strlen(fragmentShaderCode) > 0)
        {
            // Compile fragment shader
            //std::cout << "Compiling fragment shader." << std::endl;
            glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
            glCompileShader(fragmentShader);

            // Check fragment shader
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

            if (result == GL_FALSE && logLength > 1)
            {
                std::vector<char> shaderError((logLength > 1) ? logLength : 1);
                glGetShaderInfoLog(fragmentShader, logLength, nullptr, &shaderError[0]);
                std::cout << "Fragment Shader Error: " << &shaderError[0] << "(" << fragmentShaderCode << ")" << std::endl;
            }
        }

        // Program
        //std::cout << "Linking program" << std::endl;
        shaderProgram = glCreateProgram();

        if (vertextShaderCode && strlen(vertextShaderCode) > 0)
        {
            glAttachShader(shaderProgram, vertexShader);
        }
        if (fragmentShaderCode && strlen(fragmentShaderCode) > 0)
        {
            glAttachShader(shaderProgram, fragmentShader);
        }

        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

        if (result == GL_FALSE && logLength > 1)
        {
            std::vector<char> programError( (logLength > 1) ? logLength : 1 );
            glGetProgramInfoLog(shaderProgram, logLength, nullptr, &programError[0]);
            std::cout << "Link Shader Program Error: " << &programError[0] << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void kShader::use()
    {
        glUseProgram(shaderProgram);
    }

    void kShader::unuse()
    {
        glUseProgram(0);
    }

    void kShader::setShaderProgram(GLuint program)
    {
        shaderProgram = program;
    }

    GLuint kShader::getShaderProgram()
    {
        return shaderProgram;
    }

    void kShader::setValue(std::string name, std::vector<glm::mat4> value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniformMatrix4fv(loc, static_cast<GLsizei>(value.size()), GL_FALSE, glm::value_ptr(value[0]));
    }

    void kShader::setValue(std::string name, glm::mat4 value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }

    void kShader::setValue(std::string name, glm::vec3 value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform3fv(loc, 1, glm::value_ptr(value));
    }

    void kShader::setValue(std::string name, glm::vec2 value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform2fv(loc, 1, glm::value_ptr(value));
    }

    void kShader::setValue(std::string name, float value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform1f(loc, value);
    }

    void kShader::setValue(std::string name, int value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform1i(loc, value);
    }

    void kShader::setValue(std::string name, unsigned int value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform1i(loc, value);
    }

    void kShader::setValue(std::string name, bool value)
    {
        GLint loc = glGetUniformLocation(shaderProgram, name.c_str());

        /*if (loc == -1)
        {
            std::cerr << "Uniform '" << name << "' not found in shader program!" << std::endl;
            return;
        }*/

        glUniform1i(loc, value);
    }
}
