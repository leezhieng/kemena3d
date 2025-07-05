#ifndef KDATATYPE_H
#define KDATATYPE_H

#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace kemena
{
    #define MAX_BONE_INFLUENCE  4
    #define MAX_BONES           128

    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
    typedef glm::ivec4 ivec4;
    typedef glm::mat3 mat3;
    typedef glm::mat4 mat4;
    typedef glm::quat quat;

    typedef std::string string;

    enum kRendererType
    {
        RENDERER_GL
    };

    enum kNodeType
    {
        NODE_TYPE_OBJECT,
        NODE_TYPE_MESH,
        NODE_TYPE_CAMERA,
        NODE_TYPE_LIGHT
    };

    enum kCameraType
    {
        CAMERA_TYPE_FREE,
        CAMERA_TYPE_LOCKED
    };

    enum kTransparentType
    {
        TRANSP_TYPE_NONE,
        TRANSP_TYPE_BLEND
    };

    enum kLightType
    {
        LIGHT_TYPE_SUN,
        LIGHT_TYPE_POINT,
        LIGHT_TYPE_SPOT
    };

    enum kTextureFormat
    {
        TEX_FORMAT_RGB,
        TEX_FORMAT_RGBA,
        TEX_FORMAT_SRGB,
        TEX_FORMAT_SRGBA,
    };

    enum kTextureType
    {
        TEX_TYPE_2D,
        TEX_TYPE_CUBE
    };

    #define K_EVENT_QUIT            256
    #define K_EVENT_KEYDOWN         768
    #define K_EVENT_KEYUP           769
    #define K_EVENT_MOUSEMOTION     1024
    #define K_EVENT_MOUSEBUTTONDOWN 1025
    #define K_EVENT_MOUSEBUTTONUP   1026
    #define K_EVENT_MOUSEWHEEL      1027

    #define K_MOUSEBUTTON_LEFT      1
    #define K_MOUSEBUTTON_MIDDLE    2
    #define K_MOUSEBUTTON_RIGHT     3

    // Key Code (OS key location)
    #define K_KEY_UNKNOWN	    0
    #define K_KEY_BACKSPACE	    8
    #define K_KEY_TAB	        9
    #define K_KEY_RETURN	    13
    #define K_KEY_ESCAPE	    27
    #define K_KEY_SPACE	        32
    #define K_KEY_EXCLAIM	    33 // !
    #define K_KEY_QUOTEDBL	    34 // "
    #define K_KEY_HASH          35 // #
    #define K_KEY_DOLLAR	    36 // $
    #define K_KEY_PERCENT	    37 // %
    #define K_KEY_AMPERSAND	    38 // &
    #define K_KEY_QUOTE	        39 // '
    #define K_KEY_LEFTPAREN	    40 // (
    #define K_KEY_RIGHTPAREN    41 // )
    #define K_KEY_ASTERISK      42 // *
    #define K_KEY_PLUS          43 // +
    #define K_KEY_COMMA         44 // ,
    #define K_KEY_MINUS         45 // -
    #define K_KEY_PERIOD        46 // .
    #define K_KEY_SLASH         47 // /
    #define K_KEY_0             48
    #define K_KEY_1             49
    #define K_KEY_2             50
    #define K_KEY_3	            51
    #define K_KEY_4	            52
    #define K_KEY_5	            53
    #define K_KEY_6	            54
    #define K_KEY_7	            55
    #define K_KEY_8	            56
    #define K_KEY_9	            57
    #define K_KEY_A	            97
    #define K_KEY_B	            98
    #define K_KEY_C	            99
    #define K_KEY_D	            100
    #define K_KEY_E	            101
    #define K_KEY_F	            102
    #define K_KEY_G	            103
    #define K_KEY_H	            104
    #define K_KEY_I	            105
    #define K_KEY_J	            106
    #define K_KEY_K	            107
    #define K_KEY_L	            108
    #define K_KEY_M	            109
    #define K_KEY_N	            110
    #define K_KEY_O	            111
    #define K_KEY_P	            112
    #define K_KEY_Q	            113
    #define K_KEY_R	            114
    #define K_KEY_S	            115
    #define K_KEY_T	            116
    #define K_KEY_U	            117
    #define K_KEY_V	            118
    #define K_KEY_W	            119
    #define K_KEY_X	            120
    #define K_KEY_Y             121
    #define K_KEY_Z             122

    // Scan Code (physical key location)
    #define K_SCANCODE_UNKNOWN 0
    #define K_SCANCODE_A 4
    #define K_SCANCODE_B 5
    #define K_SCANCODE_C 6
    #define K_SCANCODE_D 7
    #define K_SCANCODE_E 8
    #define K_SCANCODE_F 9
    #define K_SCANCODE_G 10
    #define K_SCANCODE_H 11
    #define K_SCANCODE_I 12
    #define K_SCANCODE_J 13
    #define K_SCANCODE_K 14
    #define K_SCANCODE_L 15
    #define K_SCANCODE_M 16
    #define K_SCANCODE_N 17
    #define K_SCANCODE_O 18
    #define K_SCANCODE_P 19
    #define K_SCANCODE_Q 20
    #define K_SCANCODE_R 21
    #define K_SCANCODE_S 22
    #define K_SCANCODE_T 23
    #define K_SCANCODE_U 24
    #define K_SCANCODE_V 25
    #define K_SCANCODE_W 26
    #define K_SCANCODE_X 27
    #define K_SCANCODE_Y 28
    #define K_SCANCODE_Z 29
    #define K_SCANCODE_1 30
    #define K_SCANCODE_2 31
    #define K_SCANCODE_3 32
    #define K_SCANCODE_4 33
    #define K_SCANCODE_5 34
    #define K_SCANCODE_6 35
    #define K_SCANCODE_7 36
    #define K_SCANCODE_8 37
    #define K_SCANCODE_9 38
    #define K_SCANCODE_0 39
    #define K_SCANCODE_RETURN 40
    #define K_SCANCODE_ESCAPE 41
    #define K_SCANCODE_BACKSPACE 42
    #define K_SCANCODE_TAB 43
    #define K_SCANCODE_SPACE 44
    #define K_SCANCODE_MINUS 45
    #define K_SCANCODE_EQUALS 46
    #define K_SCANCODE_LEFTBRACKET 47
    #define K_SCANCODE_RIGHTBRACKET 48
    #define K_SCANCODE_BACKSLASH 49
    #define K_SCANCODE_NONUSHASH 50
    #define K_SCANCODE_SEMICOLON 51
    #define K_SCANCODE_APOSTROPHE 52
    #define K_SCANCODE_GRAVE 53
    #define K_SCANCODE_COMMA 54
    #define K_SCANCODE_PERIOD 55
    #define K_SCANCODE_SLASH 56
    #define K_SCANCODE_CAPSLOCK 57
    #define K_SCANCODE_F1 58
    #define K_SCANCODE_F2 59
    #define K_SCANCODE_F3 60
    #define K_SCANCODE_F4 61
    #define K_SCANCODE_F5 62
    #define K_SCANCODE_F6 63
    #define K_SCANCODE_F7 64
    #define K_SCANCODE_F8 65
    #define K_SCANCODE_F9 66
    #define K_SCANCODE_F10 67
    #define K_SCANCODE_F11 68
    #define K_SCANCODE_F12 69
    #define K_SCANCODE_PRINTSCREEN 70
    #define K_SCANCODE_SCROLLLOCK 71
    #define K_SCANCODE_PAUSE 72
    #define K_SCANCODE_INSERT 73
    #define K_SCANCODE_HOME 74
    #define K_SCANCODE_PAGEUP 75
    #define K_SCANCODE_DELETE 76
    #define K_SCANCODE_END 77
    #define K_SCANCODE_PAGEDOWN 78
    #define K_SCANCODE_RIGHT 79
    #define K_SCANCODE_LEFT 80
    #define K_SCANCODE_DOWN 81
    #define K_SCANCODE_UP 82
    #define K_SCANCODE_NUMLOCKCLEAR 83
    #define K_SCANCODE_KP_DIVIDE 84
    #define K_SCANCODE_KP_MULTIPLY 85
    #define K_SCANCODE_KP_MINUS 86
    #define K_SCANCODE_KP_PLUS 87
    #define K_SCANCODE_KP_ENTER 88
    #define K_SCANCODE_KP_1 89
    #define K_SCANCODE_KP_2 90
    #define K_SCANCODE_KP_3 91
    #define K_SCANCODE_KP_4 92
    #define K_SCANCODE_KP_5 93
    #define K_SCANCODE_KP_6 94
    #define K_SCANCODE_KP_7 95
    #define K_SCANCODE_KP_8 96
    #define K_SCANCODE_KP_9 97
    #define K_SCANCODE_KP_0 98
    #define K_SCANCODE_KP_PERIOD 99
    #define K_SCANCODE_NONUSBACKSLASH 100
    #define K_SCANCODE_APPLICATION 101
    #define K_SCANCODE_KP_EQUALS 103
    #define K_SCANCODE_F13 104
    #define K_SCANCODE_F14 105
    #define K_SCANCODE_F15 106
    #define K_SCANCODE_F16 107
    #define K_SCANCODE_F17 108
    #define K_SCANCODE_F18 109
    #define K_SCANCODE_F19 110
    #define K_SCANCODE_F20 111
    #define K_SCANCODE_F21 112
    #define K_SCANCODE_F22 113
    #define K_SCANCODE_F23 114
    #define K_SCANCODE_F24 115
    #define K_SCANCODE_EXECUTE 116
    #define K_SCANCODE_HELP 117
    #define K_SCANCODE_MENU 118
    #define K_SCANCODE_SELECT 119
    #define K_SCANCODE_STOP 120
    #define K_SCANCODE_AGAIN 121
    #define K_SCANCODE_UNDO 122
    #define K_SCANCODE_CUT 123
    #define K_SCANCODE_COPY 124
    #define K_SCANCODE_PASTE 125
    #define K_SCANCODE_FIND 126
    #define K_SCANCODE_MUTE 127
    #define K_SCANCODE_VOLUMEUP 128
    #define K_SCANCODE_VOLUMEDOWN 129
    #define K_SCANCODE_KP_COMMA 133
    #define K_SCANCODE_KP_EQUALSAS400 134
    #define K_SCANCODE_LCTRL 224
    #define K_SCANCODE_LSHIFT 225
    #define K_SCANCODE_LALT 226
    #define K_SCANCODE_LGUI 227
    #define K_SCANCODE_RCTRL 228
    #define K_SCANCODE_RSHIFT 229
    #define K_SCANCODE_RALT 230
    #define K_SCANCODE_RGUI 231

    struct kSystemEvent
    {
        SDL_Event event;
        std::vector<unsigned int> keys;

        bool hasEvent()
        {
            if (SDL_PollEvent(&event))
            {
                return true;
            }
            return false;
        }

        unsigned int getType()
        {
            if (event.type == K_EVENT_KEYDOWN)
            {
                unsigned int key = getKeyButton();

                // Put key into array
                bool found = false;
                if (keys.size() > 0)
                {
                    for (size_t i = 0; i < keys.size(); ++i)
                    {
                        if (keys.at(i) == key)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                {
                    keys.push_back(key);
                }
            }
            else if (event.type == K_EVENT_KEYUP)
            {
                unsigned int key = getKeyButton();

                // Remove key from array
                if (keys.size() > 0)
                {
                    for (size_t i = keys.size() - 1; i >= 0; --i)
                    {
                        if (keys.at(i) == key)
                        {
                            keys.erase(keys.begin() + i);
                            break;
                        }
                    }
                }
            }

            return event.type;
        }

        unsigned int getMouseButton()
        {
            return event.button.button;
        }

        float getMouseX()
        {
            return event.button.x;
        }

        float getMouseY()
        {
            return event.button.y;
        }

        float getMouseMoveX()
        {
            return event.motion.x;
        }

        float getMouseMoveY()
        {
            return event.motion.y;
        }

        unsigned int getKeyButton()
        {
            return event.key.key;
        }

        bool getKeyDown(unsigned int key)
        {
            bool found = false;
            if (keys.size() > 0)
            {
                for (size_t i = 0; i < keys.size(); ++i)
                {
                    if (keys.at(i) == key)
                    {
                        found = true;
                        break;
                    }
                }
            }
            return found;
        }

        float getMouseWheelX()
        {
            return event.wheel.x;
        }

        float getMouseWheelY()
        {
            return event.wheel.y;
        }
    };

    struct kVertexInfo
    {
        // position
        vec3 position;
        // normal
        vec3 normal;
        // texCoords
        vec2 texCoords;

        // tangent
        vec3 tangent;
        // bitangent
        vec3 bitangent;

        //bone indexes which will influence this vertex
        int boneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float weights[MAX_BONE_INFLUENCE];

    };

    struct kBoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        mat4 offset;
    };

    struct kKeyPosition
    {
        vec3 position;
        float timeStamp;
    };

    struct kKeyRotation
    {
        quat orientation;
        float timeStamp;
    };

    struct kKeyScale
    {
        vec3 scale;
        float timeStamp;
    };

    struct kAssimpNodeData
    {
        mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<kAssimpNodeData> children;
    };

    struct kFontGlyph
    {
        float ax; // advance.x
        float lsb; // left side bearing
        float tx, ty; // texture coordinates
        int w, h, xoff, yoff;
    };

    inline std::string generateUuid()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 15);
        std::uniform_int_distribution<uint32_t> dis2(8, 11); // for variant

        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < 8; ++i) ss << dis(gen);
        ss << "-";
        for (int i = 0; i < 4; ++i) ss << dis(gen);
        ss << "-4"; // UUID version 4
        for (int i = 0; i < 3; ++i) ss << dis(gen);
        ss << "-";
        ss << dis2(gen); // variant 10xx
        for (int i = 0; i < 3; ++i) ss << dis(gen);
        ss << "-";
        for (int i = 0; i < 12; ++i) ss << dis(gen);
        return ss.str();
    }

    class kAssimpGLMHelpers
    {
    public:

        static inline mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            mat4 to;
            //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }

        static inline vec2 getGLMVec2(const aiVector3D& vec)
        {
            return vec2(vec.x, vec.y);
        }

        static inline vec3 getGLMVec3(const aiVector3D& vec)
        {
            return vec3(vec.x, vec.y, vec.z);
        }

        static inline quat getGLMQuat(const aiQuaternion& pOrientation)
        {
            return quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
    };
};

#endif // KDATATYPE_H

