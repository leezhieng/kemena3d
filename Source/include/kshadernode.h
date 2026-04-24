#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include "kdatatype.h"
#include "nlohmann/json.hpp"

namespace kemena
{

// ---------------------------------------------------------------------------
// Pin / connection types
// ---------------------------------------------------------------------------

enum class kPinType
{
    Float,
    Vec2,
    Vec3,
    Vec4,
    Sampler2D,
    SamplerCube,
};

inline const char* kPinTypeName(kPinType t)
{
    switch (t)
    {
        case kPinType::Float:       return "float";
        case kPinType::Vec2:        return "vec2";
        case kPinType::Vec3:        return "vec3";
        case kPinType::Vec4:        return "vec4";
        case kPinType::Sampler2D:   return "sampler2D";
        case kPinType::SamplerCube: return "samplerCube";
    }
    return "float";
}

// Returns true if fromType can feed into toType (includes safe promotions)
inline bool kPinCompatible(kPinType from, kPinType to)
{
    if (from == to) return true;
    // float → any numeric
    if (from == kPinType::Float &&
        (to == kPinType::Vec2 || to == kPinType::Vec3 || to == kPinType::Vec4))
        return true;
    // vec3 → vec4
    if (from == kPinType::Vec3 && to == kPinType::Vec4) return true;
    // vec4 → vec3 (take .rgb)
    if (from == kPinType::Vec4 && to == kPinType::Vec3) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Shader node types  (kShaderNodeType to avoid clash with kNodeType in kdatatype.h)
// ---------------------------------------------------------------------------

enum class kShaderNodeType
{
    // --- Constant / built-in inputs ---
    ConstFloat,
    ConstVec2,
    ConstVec3,
    ConstVec4,
    UVCoord,
    Time,
    VertexColor,
    WorldPosition,
    ViewDirection,
    VertexNormal,

    // --- Texture sampling ---
    Texture2D,
    TextureCube,

    // --- Arithmetic ---
    Add,
    Subtract,
    Multiply,
    Divide,
    Dot,
    Cross,
    Normalize,
    Length,
    Clamp,
    Mix,
    Pow,
    Abs,
    Floor,
    Ceil,
    Fract,
    Sqrt,
    Min,
    Max,
    Step,
    Smoothstep,
    OneMinus,

    // --- Vector construction / decomposition ---
    Split,
    Combine,
    Swizzle,

    // --- Material property inputs ---
    MaterialTiling,
    MaterialAmbient,
    MaterialDiffuse,
    MaterialSpecular,
    MaterialShininess,
    MaterialMetallic,
    MaterialRoughness,

    // --- Output (base shader type) ---
    OutputFlat,
    OutputPhong,
    OutputPBR,
};

// ---------------------------------------------------------------------------
// Pin descriptor
// ---------------------------------------------------------------------------

struct kShaderPin
{
    int      id       = 0;
    kString  name;
    kPinType type     = kPinType::Float;
    bool     isOutput = false;

    // Default value when no link feeds this pin
    float    defFloat    = 0.0f;
    float    defVec[4]   = { 0.f, 0.f, 0.f, 1.f };

    // Runtime UI position (screen space, set during draw)
    float    uiX = 0, uiY = 0;
};

// ---------------------------------------------------------------------------
// Node
// ---------------------------------------------------------------------------

struct kShaderNode
{
    int                     id   = 0;
    kShaderNodeType         type = kShaderNodeType::ConstFloat;
    kString                 name;
    float                   posX = 200.f, posY = 200.f;

    std::vector<kShaderPin> inputs;
    std::vector<kShaderPin> outputs;

    // Node-specific payload
    float   valueFloat[4] = { 0.f, 0.f, 0.f, 1.f }; // for Const* nodes
    kString valueStr;                                  // texture uniform name / swizzle mask
    bool    valueBool = false;

    static const char* typeName(kShaderNodeType t);
};

// ---------------------------------------------------------------------------
// Link (directed edge: output pin → input pin)
// ---------------------------------------------------------------------------

struct kShaderLink
{
    int id       = 0;
    int fromNode = 0, fromPin = 0;
    int toNode   = 0, toPin   = 0;
};

// ---------------------------------------------------------------------------
// Graph
// ---------------------------------------------------------------------------

struct kShaderGraph
{
    kString                    uuid;
    kString                    name;
    std::vector<kShaderNode>   nodes;
    std::vector<kShaderLink>   links;
    int                        nextId = 1;
    bool                       dirty  = false;

    int newId() { return nextId++; }

    kShaderNode*       findNode(int id);
    const kShaderNode* findNode(int id) const;

    // Returns the link whose toNode/toPin equals the given input pin, or nullptr
    const kShaderLink* incomingLink(int nodeId, int pinId) const;

    bool isPinConnected(int nodeId, int pinId) const;

    // Remove all links touching a node or a specific pin
    void removeLinksByNode(int nodeId);
    void removeLinksByPin(int nodeId, int pinId);

    // Factory: create a node of a given type with default pins wired up
    kShaderNode makeNode(kShaderNodeType type, float x, float y);

    // JSON serialization
    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& j);
};

// ---------------------------------------------------------------------------
// Compiler: walks the graph and emits combined GLSL source
// ---------------------------------------------------------------------------

struct kShaderCompileResult
{
    bool        success = false;
    kString     error;
    kString     glsl;           // Combined (vertex + fragment)

    // Auto-detected material inputs (for material inspector display)
    bool        usesAlbedoMap            = false;
    bool        usesNormalMap            = false;
    bool        usesMetallicRoughnessMap = false;
    bool        usesAoMap                = false;
    bool        usesEmissiveMap          = false;
    std::vector<std::pair<kString, kString>> customSamplers; // { uniformName, "2D"|"Cube" }
    bool        needsMaterial            = false;   // uses material.* uniforms
    bool        needsLights              = false;   // OutputPhong or OutputPBR
};

class kShaderCompiler
{
public:
    static kShaderCompileResult compile(const kShaderGraph& graph);

private:
    struct Ctx
    {
        std::map<int, kString>  nodeVar;    // nodeId → emitted variable name
        std::vector<kString>    samplers;   // sampler declarations
        std::vector<kString>    uniforms;   // other uniform declarations
        std::set<kString>       emittedSamplerNames;
        std::vector<kString>    body;       // statement lines in main()
        int                     counter = 0;
        kString newVar() { return "v_" + std::to_string(counter++); }
    };

    // Emit the expression for a specific input pin of a node.
    // Returns the GLSL expression string (could be a variable name or a literal).
    static kString emitPin(const kShaderGraph& g, int nodeId, int pinId, Ctx& ctx);

    // Emit the node; return the output variable name (first output pin).
    static kString emitNode(const kShaderGraph& g, int nodeId, Ctx& ctx);

    // Promote expression from actualType to targetType if compatible
    static kString promote(const kString& expr, kPinType actual, kPinType target);

    static kString pinDefault(const kShaderPin& pin);
    static kString glslType(kPinType t) { return kPinTypeName(t); }

    static kString vertexTemplate();
    static kString fragmentPreamble(const kShaderGraph& g, const kShaderNode& outNode,
                                    const Ctx& ctx);
    static kString lightingCode(kShaderNodeType outType);
};

} // namespace kemena
