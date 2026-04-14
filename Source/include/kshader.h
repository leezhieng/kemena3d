/**
 * @file kshader.h
 * @brief GLSL shader program wrapper.
 */

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
    /**
     * @brief Specifies the output language/format produced by the Slang compiler.
     *
     * Pass one of these to loadSlangFile() or loadSlangCode() to control what the
     * Slang front-end emits before handing the result to the active kDriver.
     */
    enum class kSlangTarget
    {
        GLSL,  ///< Cross-compile to GLSL (OpenGL / Vulkan GLSL; most compatible with current backend).
        SPIRV, ///< Cross-compile to SPIR-V binary (requires OpenGL 4.6 / ARB_gl_spirv or Vulkan).
        HLSL,  ///< Cross-compile to HLSL source (Direct3D 11/12).
        DXIL,  ///< Cross-compile to DXIL binary (Direct3D 12, requires DXC).
        DXBC,  ///< Cross-compile to DXBC binary (Direct3D 11, requires FXC).
    };


    /**
     * @brief Wraps a compiled GLSL shader program.
     *
     * Shaders can be loaded from source files or inline code strings.
     * All GPU operations are dispatched through kDriver::getCurrent(), so no
     * GL headers need to be included in client code.
     *
     * Example:
     * @code
     *   kShader shader;
     *   shader.loadShadersFile("vertex.glsl", "fragment.glsl");
     *   shader.use();
     *   shader.setValue("modelMatrix", modelMat);
     *   // draw ...
     *   shader.unuse();
     * @endcode
     */
    class KEMENA3D_API kShader
    {
    public:
        kShader();
        virtual ~kShader();

        /**
         * @brief Reads a text file into a kString.
         * @param filePath Path to the file.
         * @return File contents, or empty kString if the file could not be opened.
         */
        kString readFile(const kString filePath);

        /**
         * @brief Compiles and links a shader program from source files on disk.
         * @param vertexShaderPath   Path to the GLSL vertex shader source file.
         * @param fragmentShaderPath Path to the GLSL fragment shader source file.
         */
        void loadShadersFile(const kString vertexShaderPath, const kString fragmentShaderPath);

        /**
         * @brief Compiles and links a shader program from inline source strings.
         * @param vertexShaderCode   Null-terminated GLSL vertex shader source, or nullptr.
         * @param fragmentShaderCode Null-terminated GLSL fragment shader source, or nullptr.
         */
        void loadShadersCode(const char *vertexShaderCode, const char *fragmentShaderCode);

        // --- Slang shaders ---------------------------------------------------

        /**
         * @brief Compiles a Slang source file and loads it into the active driver.
         *
         * Slang is used as a universal shading language front-end.  The source is
         * compiled to @p target (GLSL, SPIR-V, HLSL, …) and then handed to the
         * current kDriver for linking into a GPU program.
         *
         * @param filePath   Path to the @c .slang source file on disk.
         * @param vertEntry  Name of the vertex entry-point function (default: "vertexMain").
         * @param fragEntry  Name of the fragment entry-point function (default: "fragmentMain").
         * @param target     Output shading language / binary format (default: GLSL).
         */
        void loadSlangFile(const kString& filePath,
                           const kString& vertEntry = "vertexMain",
                           const kString& fragEntry = "fragmentMain",
                           kSlangTarget target = kSlangTarget::GLSL);

        /**
         * @brief Compiles an inline Slang source string and loads it into the active driver.
         *
         * @param source    Null-terminated Slang source code.
         * @param vertEntry Name of the vertex entry-point function (default: "vertexMain").
         * @param fragEntry Name of the fragment entry-point function (default: "fragmentMain").
         * @param target    Output shading language / binary format (default: GLSL).
         */
        void loadSlangCode(const kString& source,
                           const kString& vertEntry = "vertexMain",
                           const kString& fragEntry = "fragmentMain",
                           kSlangTarget target = kSlangTarget::GLSL);

        /** @brief Binds this shader program for subsequent draw calls. */
        void use();

        /** @brief Unbinds the current shader program. */
        void unuse();

        /**
         * @brief Overrides the internal program handle.
         *
         * Use this when the program was compiled externally (e.g. via kDriver).
         * @param program Compiled program handle.
         */
        void setShaderProgram(uint32_t program);

        /**
         * @brief Returns the underlying program handle.
         * @return Opaque GPU handle (0 if not yet compiled).
         */
        uint32_t getShaderProgram();

        /** @brief Sets a kMat4 array uniform. */
        void setValue(kString name, std::vector<kMat4> value);
        /** @brief Sets a kMat4 uniform. */
        void setValue(kString name, kMat4 value);
        /** @brief Sets a kVec3 uniform. */
        void setValue(kString name, kVec3 value);
        /** @brief Sets a kVec2 uniform. */
        void setValue(kString name, kVec2 value);
        /** @brief Sets a float uniform. */
        void setValue(kString name, float value);
        /** @brief Sets an integer uniform. */
        void setValue(kString name, int value);
        /** @brief Sets an unsigned-integer uniform. */
        void setValue(kString name, unsigned int value);
        /** @brief Sets a boolean uniform (uploaded as int). */
        void setValue(kString name, bool value);

    protected:
    private:
        uint32_t shaderProgram = 0; ///< Compiled program handle; 0 until loaded.
    };
}

#endif // KSHADER_H
