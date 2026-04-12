/**
 * @file kscriptmanager.h
 * @brief AngelScript engine wrapper for loading and executing scripts.
 */

#ifndef KSCRIPTMANAGER_H
#define KSCRIPTMANAGER_H

#include "kexport.h"

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
    /**
     * @brief Descriptor for a single registered script function.
     *
     * Stores both the AngelScript function pointer and the native C function
     * pointer so that the binding can be recreated after a reload.
     */
    struct KEMENA3D_API kScriptFunc
    {
        string             decl;   ///< AngelScript function declaration string.
        asIScriptFunction *asFunc; ///< Compiled AngelScript function handle.
        asSFuncPtr         cFunc;  ///< Native C function pointer for re-registration.
    };

    /**
     * @brief Represents one loaded AngelScript module (script file).
     *
     * Holds all AngelScript objects needed to compile and execute a single
     * script file.  Call call() to invoke a function by its declaration string,
     * or run() to execute all functions in the module sequentially.
     */
    struct KEMENA3D_API kScript
    {
        string uuid;             ///< Script node UUID.
        bool   isActive = true;  ///< Whether the script is executed each frame.
        string checksum;         ///< File checksum used to detect changes.

        string             fileName;   ///< Source file path.
        string             moduleName; ///< AngelScript module identifier.
        asIScriptEngine   *engine;     ///< Shared script engine.
        asIScriptContext  *context;    ///< Execution context for this script.
        asIScriptModule   *module;     ///< Compiled module.
        bool               loaded = false; ///< true once the script has been compiled.

        /**
         * @brief Calls a single function by its AngelScript declaration.
         * @param declaration Full function declaration, e.g. @c "void onUpdate()".
         */
        void call(string declaration)
        {
            context->Prepare(module->GetFunctionByDecl(declaration.c_str()));

            int result = context->Execute();
            if (result != asEXECUTION_FINISHED)
            {
                if (result == asEXECUTION_EXCEPTION)
                {
                    printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
                }
            }
        }

        /**
         * @brief Executes every function defined in the module sequentially.
         *
         * Intended for quick testing only; prefer calling individual functions
         * by declaration in production code.
         */
        void run()
        {
            if (module->GetFunctionCount() > 0)
            {
                for (int j = 0; j < (int)module->GetFunctionCount(); ++j)
                {
                    context->Prepare(module->GetFunctionByIndex(j));

                    int result = context->Execute();
                    if (result != asEXECUTION_FINISHED)
                    {
                        if (result == asEXECUTION_EXCEPTION)
                        {
                            printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
                        }
                    }
                }
            }
        }

        /**
         * @brief Releases the execution context.
         *
         * Must be called before the owning kScriptManager is destroyed if the
         * script is no longer needed.
         */
        void destroy()
        {
            context->Release();
        }
    };

    /**
     * @brief Manages an AngelScript engine instance and a list of loaded scripts.
     *
     * Initialises the AngelScript engine, registers standard string support,
     * and provides methods to load scripts from disk and register native C
     * functions that can be called from scripts.
     *
     * Example:
     * @code
     *   kScriptManager mgr;
     *   mgr.registerGlobalFunction("void print(string)", FUNCTION(myPrint));
     *   kScript s = mgr.loadScript("game.as");
     *   s.call("void onStart()");
     * @endcode
     */
    class KEMENA3D_API kScriptManager
    {
    public:
        kScriptManager();
        virtual ~kScriptManager();

        /**
         * @brief Compiles and loads a script from a file.
         * @param fileName Path to the AngelScript source file (.as).
         * @return kScript descriptor ready for execution.
         */
        kScript loadScript(string fileName);

        /**
         * @brief Registers a native C function as a global callable from scripts.
         * @param declaration AngelScript function declaration string.
         * @param func        Pointer to the native function (use the FUNCTION macro).
         */
        void registerGlobalFunction(string declaration, asSFuncPtr func);

        /**
         * @brief Executes all functions in all loaded scripts sequentially.
         */
        void runAll();

        /**
         * @brief Shuts down the AngelScript engine and releases all resources.
         */
        void destroy();

    protected:
    private:
        asIScriptEngine     *engine;  ///< Shared AngelScript engine instance.
        std::vector<kScript> scripts; ///< All loaded script modules.
    };
}

#endif // KSCRIPTMANAGER_H
