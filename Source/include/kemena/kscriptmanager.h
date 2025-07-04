#ifndef KSCRIPTMANAGER_H
#define KSCRIPTMANAGER_H

#include <cassert>
#include <vector>
#include <iostream>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include "kdatatype.h"

#define FUNCTION asFUNCTION

namespace kemena
{
    struct kScriptFunc
    {
        std::string decl;
        asIScriptFunction* asFunc;
        asSFuncPtr cFunc;

        /*
        void call(asIScriptContext* context)
        {
            context->Prepare(asFunc);

            int result = context->Execute();
            if(result != asEXECUTION_FINISHED)
            {
                // The execution didn't complete as expected. Determine what happened.
                if(result == asEXECUTION_EXCEPTION)
                {
                    // An exception occurred, let the script writer know what happened so it can be corrected.
                    printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
                }
            }
        }
        */
    };

    struct kScript
    {
        std::string uuid;
        bool isActive = true;
        std::string checksum;

        std::string fileName;
        std::string moduleName;
        asIScriptEngine* engine;
        asIScriptContext* context;
        asIScriptModule* module;
        //std::vector<kScriptFunc> functions;
        bool loaded = false;

        void call(std::string declaration)
        {
            context->Prepare(module->GetFunctionByDecl(declaration.c_str()));

            int result = context->Execute();
            if(result != asEXECUTION_FINISHED)
            {
                // The execution didn't complete as expected. Determine what happened.
                if(result == asEXECUTION_EXCEPTION)
                {
                    // An exception occurred, let the script writer know what happened so it can be corrected.
                    printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
                }
            }
        }

        // Run all functions
        // For quick test only, not recommended to use
        // Recommended to call each function manually
        void run()
        {
            if (module->GetFunctionCount() > 0)
            {
                for (int j = 0; j < (int) module->GetFunctionCount(); ++j)
                {
                    context->Prepare(module->GetFunctionByIndex(j));

                    int result = context->Execute();
                    if(result != asEXECUTION_FINISHED)
                    {
                        // The execution didn't complete as expected. Determine what happened.
                        if(result == asEXECUTION_EXCEPTION)
                        {
                            // An exception occurred, let the script writer know what happened so it can be corrected.
                            printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
                        }
                    }
                }
            }
        }

        void destroy()
        {
            context->Release();
        }
    };

    class kScriptManager
    {
        public:
            kScriptManager();
            virtual ~kScriptManager();

            kScript loadScript(std::string fileName);
            void registerGlobalFunction(std::string declaration, asSFuncPtr func);
            void runAll();
            void destroy();

        protected:

        private:
            asIScriptEngine* engine;
            std::vector<kScript> scripts;
    };
}

#endif // KSCRIPTMANAGER_H
