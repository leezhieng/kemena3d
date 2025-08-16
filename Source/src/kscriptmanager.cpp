#include "kscriptmanager.h"

namespace kemena
{
    // Implement a simple message callback function
    static void messageCallback(const asSMessageInfo *msg, void *param)
    {
        const char *type = "ERR ";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";
        printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
    }

    kScriptManager::kScriptManager()
    {
        engine = asCreateScriptEngine();

        int result = engine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL);
        assert(result >= 0);

        RegisterStdString(engine);
    }

    kScriptManager::~kScriptManager()
    {
    }

    kScript kScriptManager::loadScript(std::string fileName)
    {
        // Auto generate module name by removing ".as"
        std::string moduleName = fileName;
        if (fileName.substr(moduleName.size() - 3, moduleName.size()) == ".as")
            moduleName = moduleName.substr(0, moduleName.size() - 3);

        kScript newScript;
        newScript.fileName = fileName;
        newScript.moduleName = moduleName;

        newScript.uuid = generateUuid();
        // WIP: set checksum

        CScriptBuilder builder;
        int result = builder.StartNewModule(engine, moduleName.c_str());
        if (result < 0)
        {
            // If the code fails here it is usually because there
            // is no more memory to allocate the module
            printf("Unrecoverable error while starting a new module.\n");
            return newScript;
        }
        result = builder.AddSectionFromFile(fileName.c_str());
        if (result < 0)
        {
            // The builder wasn't able to load the file. Maybe the file
            // has been removed, or the wrong name was given, or some
            // preprocessing commands are incorrectly written.
            printf("Please correct the errors in the script and try again.\n");
            return newScript;
        }
        result = builder.BuildModule();
        if (result < 0)
        {
            // An error occurred. Instruct the script writer to fix the
            // compilation errors that were listed in the output stream.
            printf("Please correct the errors in the script and try again.\n");
            return newScript;
        }

        newScript.engine = engine;
        newScript.module = engine->GetModule(moduleName.c_str());
        newScript.context = engine->CreateContext();
        newScript.loaded = true;

        // Auto look for variables and functions
        int varCount = (int)newScript.module->GetGlobalVarCount();
        int funcCount = (int)newScript.module->GetFunctionCount();
        int impFuncCount = (int)newScript.module->GetImportedFunctionCount();
        int enumCount = (int)newScript.module->GetEnumCount();
        int objTypeCount = (int)newScript.module->GetObjectTypeCount();
        int typeDefCount = (int)newScript.module->GetTypedefCount();

        // std::cout << varCount << std::endl;

        scripts.push_back(newScript);

        return newScript;
    }

    void kScriptManager::registerGlobalFunction(std::string declaration, asSFuncPtr func)
    {
        // Register the function that we want the scripts to call
        int result = engine->RegisterGlobalFunction(declaration.c_str(), func, asCALL_CDECL);
        assert(result >= 0);
    }

    // Run all functions in all scripts
    // For quick test only, not recommended to use
    // Recommended to run each script manually
    void kScriptManager::runAll()
    {
        if (scripts.size() > 0)
        {
            for (size_t i = 0; i < scripts.size(); ++i)
            {
                if (scripts.at(i).module->GetFunctionCount() > 0)
                {
                    for (int j = 0; j < (int)scripts.at(i).module->GetFunctionCount(); ++j)
                    {
                        scripts.at(i).context->Prepare(scripts.at(i).module->GetFunctionByIndex(j));

                        int result = scripts.at(i).context->Execute();
                        if (result != asEXECUTION_FINISHED)
                        {
                            // The execution didn't complete as expected. Determine what happened.
                            if (result == asEXECUTION_EXCEPTION)
                            {
                                // An exception occurred, let the script writer know what happened so it can be corrected.
                                printf("An exception '%s' occurred. Please correct the code and try again.\n", scripts.at(i).context->GetExceptionString());
                            }
                        }
                    }
                }
            }
        }
    }

    void kScriptManager::destroy()
    {
        if (scripts.size() > 0)
        {
            for (size_t i = 0; i < scripts.size(); ++i)
            {
                scripts.at(i).destroy();
            }
        }
        engine->ShutDownAndRelease();
    }
}
