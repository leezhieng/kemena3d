#include "kshader.h"

#include <slang.h>
#include <slang-com-ptr.h>

#include <mutex>

namespace kemena
{
    // -------------------------------------------------------------------------
    // Slang global session – created once, shared across all kShader instances.
    // -------------------------------------------------------------------------
    static slang::IGlobalSession *getSlangGlobalSession()
    {
        static Slang::ComPtr<slang::IGlobalSession> s_session;
        static std::once_flag s_flag;
        std::call_once(s_flag, []()
        {
            if (SLANG_FAILED(slang_createGlobalSession(SLANG_API_VERSION, s_session.writeRef())))
                std::cout << "[kShader] Failed to create Slang global session." << std::endl;
        });
        return s_session.get();
    }


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

    kString kShader::readFile(const kString filePath)
    {
        kString content;
        std::ifstream fileStream(filePath.c_str(), std::ios::in);

        if (!fileStream.is_open())
        {
            std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
            return "";
        }

        kString line = "";
        while (!fileStream.eof())
        {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }

    void kShader::loadShadersFile(const kString vertexShaderPath, const kString fragmentShaderPath)
    {
        const char *vertSrc = nullptr;
        const char *fragSrc = nullptr;
        kString vertStr, fragStr;

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

    void kShader::setValue(kString name, std::vector<kMat4> value)
    {
        kDriver::getCurrent()->setUniformMat4Array(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, kMat4 value)
    {
        kDriver::getCurrent()->setUniformMat4(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, kVec3 value)
    {
        kDriver::getCurrent()->setUniformVec3(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, kVec2 value)
    {
        kDriver::getCurrent()->setUniformVec2(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, float value)
    {
        kDriver::getCurrent()->setUniformFloat(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, int value)
    {
        kDriver::getCurrent()->setUniformInt(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, unsigned int value)
    {
        kDriver::getCurrent()->setUniformUint(shaderProgram, name, value);
    }

    void kShader::setValue(kString name, bool value)
    {
        kDriver::getCurrent()->setUniformBool(shaderProgram, name, value);
    }

    // -------------------------------------------------------------------------
    // Slang compilation
    // -------------------------------------------------------------------------

    void kShader::loadSlangFile(const kString &filePath,
                                const kString &vertEntry,
                                const kString &fragEntry,
                                kSlangTarget target)
    {
        kString source = readFile(filePath);
        if (source.empty())
        {
            std::cout << "[kShader] loadSlangFile: could not read '" << filePath << "'." << std::endl;
            return;
        }
        loadSlangCode(source, vertEntry, fragEntry, target);
    }

    void kShader::loadSlangCode(const kString &source,
                                const kString &vertEntry,
                                const kString &fragEntry,
                                kSlangTarget target)
    {
        slang::IGlobalSession *globalSession = getSlangGlobalSession();
        if (!globalSession)
        {
            std::cout << "[kShader] Slang global session unavailable." << std::endl;
            return;
        }

        // Map kSlangTarget → Slang compile target + profile name
        SlangCompileTarget slangTarget;
        const char *profileName = nullptr;
        switch (target)
        {
            case kSlangTarget::GLSL:  slangTarget = SLANG_GLSL;  profileName = "glsl_450"; break;
            case kSlangTarget::SPIRV: slangTarget = SLANG_SPIRV; profileName = "spirv_1_3"; break;
            case kSlangTarget::HLSL:  slangTarget = SLANG_HLSL;  profileName = "sm_5_0";   break;
            case kSlangTarget::DXIL:  slangTarget = SLANG_DXIL;  profileName = "sm_6_0";   break;
            case kSlangTarget::DXBC:  slangTarget = SLANG_DXBC;  profileName = "sm_5_0";   break;
            default:                  slangTarget = SLANG_GLSL;  profileName = "glsl_450"; break;
        }

        slang::TargetDesc targetDesc = {};
        targetDesc.format  = slangTarget;
        targetDesc.profile = globalSession->findProfile(profileName);
        if (targetDesc.profile == SLANG_PROFILE_UNKNOWN)
            std::cout << "[kShader] Warning: Slang profile '" << profileName << "' not found." << std::endl;

        slang::SessionDesc sessionDesc = {};
        sessionDesc.targets      = &targetDesc;
        sessionDesc.targetCount  = 1;

        Slang::ComPtr<slang::ISession> session;
        if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef())))
        {
            std::cout << "[kShader] Failed to create Slang session." << std::endl;
            return;
        }

        // Load module from inline source string
        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IModule> slangModule;
        slangModule = session->loadModuleFromSourceString(
            "kemena_shader", "kemena_shader.slang", source.c_str(), diagnostics.writeRef());

        if (diagnostics && diagnostics->getBufferSize() > 0)
            std::cout << "[kShader] Slang diagnostics:\n"
                      << static_cast<const char *>(diagnostics->getBufferPointer()) << std::endl;

        if (!slangModule)
        {
            std::cout << "[kShader] Slang module compilation failed." << std::endl;
            return;
        }

        // Resolve entry points
        Slang::ComPtr<slang::IEntryPoint> vertEP, fragEP;
        if (SLANG_FAILED(slangModule->findEntryPointByName(vertEntry.c_str(), vertEP.writeRef())) || !vertEP)
            std::cout << "[kShader] Warning: vertex entry point '" << vertEntry << "' not found." << std::endl;
        if (SLANG_FAILED(slangModule->findEntryPointByName(fragEntry.c_str(), fragEP.writeRef())) || !fragEP)
            std::cout << "[kShader] Warning: fragment entry point '" << fragEntry << "' not found." << std::endl;

        if (!vertEP && !fragEP)
        {
            std::cout << "[kShader] No entry points found; aborting." << std::endl;
            return;
        }

        // Build composite: module + entry points
        std::vector<slang::IComponentType *> components;
        components.push_back(slangModule);
        if (vertEP) components.push_back(vertEP);
        if (fragEP) components.push_back(fragEP);

        Slang::ComPtr<slang::IComponentType> composite;
        if (SLANG_FAILED(session->createCompositeComponentType(
                components.data(), static_cast<SlangInt>(components.size()),
                composite.writeRef(), diagnostics.writeRef())))
        {
            std::cout << "[kShader] Slang compositing failed." << std::endl;
            return;
        }

        // Link
        Slang::ComPtr<slang::IComponentType> linked;
        if (SLANG_FAILED(composite->link(linked.writeRef(), diagnostics.writeRef())))
        {
            if (diagnostics && diagnostics->getBufferSize() > 0)
                std::cout << "[kShader] Link diagnostics:\n"
                          << static_cast<const char *>(diagnostics->getBufferPointer()) << std::endl;
            std::cout << "[kShader] Slang link failed." << std::endl;
            return;
        }

        // Retrieve per-stage code blobs
        // Entry point indices in the linked program follow the order they were added to the composite
        // (module holds 0 visible EPs; vertEP is index 0, fragEP is index 1).
        int epIndex = 0;
        Slang::ComPtr<slang::IBlob> vertCode, fragCode;

        if (vertEP)
        {
            if (SLANG_FAILED(linked->getEntryPointCode(epIndex, 0, vertCode.writeRef(), diagnostics.writeRef())))
                std::cout << "[kShader] Failed to get vertex entry point code." << std::endl;
            ++epIndex;
        }
        if (fragEP)
        {
            if (SLANG_FAILED(linked->getEntryPointCode(epIndex, 0, fragCode.writeRef(), diagnostics.writeRef())))
                std::cout << "[kShader] Failed to get fragment entry point code." << std::endl;
        }

        // Hand off to the active driver
        if (target == kSlangTarget::GLSL)
        {
            const char *vertSrc = vertCode ? static_cast<const char *>(vertCode->getBufferPointer()) : nullptr;
            const char *fragSrc = fragCode ? static_cast<const char *>(fragCode->getBufferPointer()) : nullptr;
            shaderProgram = kDriver::getCurrent()->compileShaderProgram(vertSrc, fragSrc);
        }
        else if (target == kSlangTarget::SPIRV)
        {
            auto toBytes = [](slang::IBlob *blob) -> std::vector<uint8_t>
            {
                if (!blob) return {};
                const auto *ptr = static_cast<const uint8_t *>(blob->getBufferPointer());
                return std::vector<uint8_t>(ptr, ptr + blob->getBufferSize());
            };
            shaderProgram = kDriver::getCurrent()->compileShaderProgramSpirv(
                toBytes(vertCode.get()), vertEntry,
                toBytes(fragCode.get()), fragEntry);
        }
        else
        {
            std::cout << "[kShader] Target " << static_cast<int>(target)
                      << " (HLSL/DXIL/DXBC) is not supported by the current backend." << std::endl;
        }
    }
}
