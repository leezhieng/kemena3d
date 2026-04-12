#ifndef KOPENGLDRIVER_H
#define KOPENGLDRIVER_H

#include "kdriver.h"
#include "kwindow.h"

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define NO_SDL_GLEXT
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <iostream>
#include <vector>

namespace kemena
{
    class KEMENA3D_API kOpenGLDriver : public kDriver
    {
    public:
        kOpenGLDriver() = default;
        ~kOpenGLDriver() override;

        bool init(kWindow *window) override;
        void destroy() override;
        void *getNativeContext() override;
        string getApiVersion() override;
        string getShaderVersion() override;

        void setClearColor(float r, float g, float b, float a) override;
        void clear(bool color, bool depth, bool stencil) override;
        void setViewport(int x, int y, int width, int height) override;

        void setDepthTest(bool enable) override;
        void setDepthWrite(bool enable) override;
        void setBlend(bool enable) override;
        void setBlendFunc(kBlendFactor src, kBlendFactor dst) override;
        void setCullFace(bool enable) override;
        void setCullMode(kCullMode mode) override;
        void setFrontFace(kFrontFace face) override;
        void setMultisample(bool enable) override;
        void setSRGBEncoding(bool enable) override;
        void setSampleAlphaToCoverage(bool enable) override;

        uint32_t compileShaderProgram(const char *vertSrc, const char *fragSrc) override;
        void deleteShaderProgram(uint32_t id) override;
        void bindShaderProgram(uint32_t id) override;
        void unbindShaderProgram() override;
        void setUniformBool(uint32_t progId, const string &name, bool v) override;
        void setUniformInt(uint32_t progId, const string &name, int v) override;
        void setUniformUint(uint32_t progId, const string &name, uint32_t v) override;
        void setUniformFloat(uint32_t progId, const string &name, float v) override;
        void setUniformVec2(uint32_t progId, const string &name, const vec2 &v) override;
        void setUniformVec3(uint32_t progId, const string &name, const vec3 &v) override;
        void setUniformMat4(uint32_t progId, const string &name, const mat4 &v) override;
        void setUniformMat4Array(uint32_t progId, const string &name, const std::vector<mat4> &v) override;

        uint32_t createVertexArray() override;
        void deleteVertexArray(uint32_t id) override;
        void bindVertexArray(uint32_t id) override;
        void unbindVertexArray() override;

        uint32_t createBuffer() override;
        void deleteBuffer(uint32_t id) override;
        void uploadIndexBuffer(uint32_t bufferId, const void *data, size_t size) override;
        void uploadVertexBuffer(uint32_t bufferId, const void *data, size_t size) override;
        void setVertexAttribFloat(int location, int components, int stride, size_t offset) override;
        void setVertexAttribInt(int location, int components, int stride, size_t offset) override;

        void drawIndexed(uint32_t vaoId, int indexCount) override;
        void drawArrays(uint32_t vaoId, kPrimitiveType type, int vertexCount) override;

        void bindTexture2D(int unit, uint32_t id) override;
        void bindTextureCube(int unit, uint32_t id) override;
        void unbindTexture2D(int unit) override;
        void unbindTextureCube(int unit) override;
        void generateMipmaps2D(uint32_t id) override;
        void readTexture2DRGB(uint32_t id, int mipLevel, float *pixels) override;

        uint32_t createFramebuffer() override;
        void deleteFramebuffer(uint32_t id) override;
        void bindFramebuffer(uint32_t id) override;
        void bindReadFramebuffer(uint32_t id) override;
        void bindDrawFramebuffer(uint32_t id) override;
        void unbindFramebuffer() override;
        bool isFramebufferComplete() override;
        void blitFramebufferColor(int srcX0, int srcY0, int srcX1, int srcY1,
                                  int dstX0, int dstY0, int dstX1, int dstY1) override;
        void setFramebufferDrawBuffer() override;

        uint32_t createRenderbuffer() override;
        void deleteRenderbuffer(uint32_t id) override;
        void setupRenderbuffer(uint32_t rboId, int width, int height) override;
        void setupRenderbufferMSAA(uint32_t rboId, int samples, int width, int height) override;
        void attachRenderbufferDepthStencil(uint32_t fboId, uint32_t rboId) override;

        uint32_t createFBOColorTexture(int width, int height) override;
        uint32_t createFBOColorTextureMSAA(int samples, int width, int height) override;
        uint32_t createFBODepthTexture(int width, int height) override;
        void deleteFBOTexture(uint32_t id) override;
        void attachFBOColorTexture(uint32_t fboId, uint32_t texId) override;
        void attachFBOColorTextureMSAA(uint32_t fboId, uint32_t texId) override;
        void attachFBODepthTexture(uint32_t fboId, uint32_t texId) override;
        void resizeFBOColorTexture(uint32_t texId, int width, int height) override;
        void resizeFBOColorTextureMSAA(uint32_t texId, int samples, int width, int height) override;

    private:
        SDL_GLContext glContext = nullptr;

        GLenum toGLBlendFactor(kBlendFactor factor);
        GLenum toGLPrimitiveType(kPrimitiveType type);
    };
}

#endif // KOPENGLDRIVER_H
