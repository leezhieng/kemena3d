#ifndef KDRIVER_H
#define KDRIVER_H

#include "kexport.h"
#include "kdatatype.h"

#include <vector>
#include <cstdint>

namespace kemena
{
    class kWindow;

    // -------------------------------------------------------------------------
    // Enums
    // -------------------------------------------------------------------------

    enum class kBlendFactor
    {
        ZERO,
        ONE,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
    };

    enum class kCullMode
    {
        BACK,
        FRONT,
        FRONT_AND_BACK,
    };

    enum class kFrontFace
    {
        CCW,
        CW,
    };

    enum class kPrimitiveType
    {
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        LINES,
        LINE_STRIP,
        POINTS,
    };

    // -------------------------------------------------------------------------
    // Abstract driver base class
    // No graphics API headers included here — keep this file API-agnostic.
    // -------------------------------------------------------------------------

    class KEMENA3D_API kDriver
    {
    public:
        virtual ~kDriver() = default;

        // --- Lifecycle -------------------------------------------------------
        virtual bool init(kWindow *window) = 0;
        virtual void destroy() = 0;
        virtual void *getNativeContext() = 0;
        virtual string getApiVersion() = 0;
        virtual string getShaderVersion() = 0;

        // --- Frame state -----------------------------------------------------
        virtual void setClearColor(float r, float g, float b, float a) = 0;
        virtual void clear(bool color, bool depth, bool stencil) = 0;
        virtual void setViewport(int x, int y, int width, int height) = 0;

        // --- Pipeline state --------------------------------------------------
        virtual void setDepthTest(bool enable) = 0;
        virtual void setDepthWrite(bool enable) = 0;
        virtual void setBlend(bool enable) = 0;
        virtual void setBlendFunc(kBlendFactor src, kBlendFactor dst) = 0;
        virtual void setCullFace(bool enable) = 0;
        virtual void setCullMode(kCullMode mode) = 0;
        virtual void setFrontFace(kFrontFace face) = 0;
        virtual void setMultisample(bool enable) = 0;
        virtual void setSRGBEncoding(bool enable) = 0;
        virtual void setSampleAlphaToCoverage(bool enable) = 0;

        // --- Shader programs -------------------------------------------------
        virtual uint32_t compileShaderProgram(const char *vertSrc, const char *fragSrc) = 0;
        virtual void deleteShaderProgram(uint32_t id) = 0;
        virtual void bindShaderProgram(uint32_t id) = 0;
        virtual void unbindShaderProgram() = 0;
        virtual void setUniformBool(uint32_t progId, const string &name, bool v) = 0;
        virtual void setUniformInt(uint32_t progId, const string &name, int v) = 0;
        virtual void setUniformUint(uint32_t progId, const string &name, uint32_t v) = 0;
        virtual void setUniformFloat(uint32_t progId, const string &name, float v) = 0;
        virtual void setUniformVec2(uint32_t progId, const string &name, const vec2 &v) = 0;
        virtual void setUniformVec3(uint32_t progId, const string &name, const vec3 &v) = 0;
        virtual void setUniformMat4(uint32_t progId, const string &name, const mat4 &v) = 0;
        virtual void setUniformMat4Array(uint32_t progId, const string &name, const std::vector<mat4> &v) = 0;

        // --- Vertex arrays ---------------------------------------------------
        virtual uint32_t createVertexArray() = 0;
        virtual void deleteVertexArray(uint32_t id) = 0;
        virtual void bindVertexArray(uint32_t id) = 0;
        virtual void unbindVertexArray() = 0;

        // --- Buffers ---------------------------------------------------------
        virtual uint32_t createBuffer() = 0;
        virtual void deleteBuffer(uint32_t id) = 0;
        virtual void uploadIndexBuffer(uint32_t bufferId, const void *data, size_t size) = 0;
        virtual void uploadVertexBuffer(uint32_t bufferId, const void *data, size_t size) = 0;
        // Sets attrib pointer on the currently-bound VAO and enables the array.
        virtual void setVertexAttribFloat(int location, int components, int stride, size_t offset) = 0;
        virtual void setVertexAttribInt(int location, int components, int stride, size_t offset) = 0;

        // --- Draw calls ------------------------------------------------------
        virtual void drawIndexed(uint32_t vaoId, int indexCount) = 0;
        virtual void drawArrays(uint32_t vaoId, kPrimitiveType type, int vertexCount) = 0;

        // --- Texture sampling (bind/unbind for shader use) -------------------
        virtual void bindTexture2D(int unit, uint32_t id) = 0;
        virtual void bindTextureCube(int unit, uint32_t id) = 0;
        virtual void unbindTexture2D(int unit) = 0;
        virtual void unbindTextureCube(int unit) = 0;
        virtual void generateMipmaps2D(uint32_t id) = 0;
        // Reads mip level into a 3-float-per-pixel RGB buffer (used for auto-exposure).
        virtual void readTexture2DRGB(uint32_t id, int mipLevel, float *pixels) = 0;

        // --- Framebuffers ----------------------------------------------------
        virtual uint32_t createFramebuffer() = 0;
        virtual void deleteFramebuffer(uint32_t id) = 0;
        virtual void bindFramebuffer(uint32_t id) = 0;
        virtual void bindReadFramebuffer(uint32_t id) = 0;
        virtual void bindDrawFramebuffer(uint32_t id) = 0;
        virtual void unbindFramebuffer() = 0;
        virtual bool isFramebufferComplete() = 0;
        virtual void blitFramebufferColor(int srcX0, int srcY0, int srcX1, int srcY1,
                                          int dstX0, int dstY0, int dstX1, int dstY1) = 0;
        virtual void setFramebufferDrawBuffer() = 0; // points to COLOR_ATTACHMENT0

        // --- Renderbuffers ---------------------------------------------------
        virtual uint32_t createRenderbuffer() = 0;
        virtual void deleteRenderbuffer(uint32_t id) = 0;
        virtual void setupRenderbuffer(uint32_t rboId, int width, int height) = 0;
        virtual void setupRenderbufferMSAA(uint32_t rboId, int samples, int width, int height) = 0;
        virtual void attachRenderbufferDepthStencil(uint32_t fboId, uint32_t rboId) = 0;

        // --- FBO-managed textures --------------------------------------------
        // These create/resize textures intended for framebuffer attachments.
        virtual uint32_t createFBOColorTexture(int width, int height) = 0;
        virtual uint32_t createFBOColorTextureMSAA(int samples, int width, int height) = 0;
        virtual uint32_t createFBODepthTexture(int width, int height) = 0;
        virtual void deleteFBOTexture(uint32_t id) = 0;
        virtual void attachFBOColorTexture(uint32_t fboId, uint32_t texId) = 0;
        virtual void attachFBOColorTextureMSAA(uint32_t fboId, uint32_t texId) = 0;
        virtual void attachFBODepthTexture(uint32_t fboId, uint32_t texId) = 0;
        virtual void resizeFBOColorTexture(uint32_t texId, int width, int height) = 0;
        virtual void resizeFBOColorTextureMSAA(uint32_t texId, int samples, int width, int height) = 0;

        // --- Global current driver -------------------------------------------
        static kDriver *getCurrent() { return s_current; }
        static void setCurrent(kDriver *driver) { s_current = driver; }

    private:
        static kDriver *s_current;
    };
}

#endif // KDRIVER_H
