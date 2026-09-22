#include <cmath>
#include <cstring>
#include <stdexcept>
#include "../DecompileHLSL.cpp"

FILE *LogFile = stderr;
bool gLogDebug = false;

string emit_typed_buffer(const string &kind, const string &reflectedType, bool array)
{
    Decompiler d;
    DecompilerSettings settings;
    d.G = &settings;
    d.mErrorOccurred = false;
    if (!reflectedType.empty()) {
        d.mTextureNames[0] = array ? "VS_t0[0]" : "VS_t0";
        d.mTextureType[0] = reflectedType;
        d.mTextureNamesArraySize[0] = array ? 2 : 1;
        if (array) d.mTextureNames[1] = "VS_t0[1]";
        d.WriteResourceDefinitions();
    }
    Shader shader = {};
    Instruction instruction = {};
    shader.asPhase[MAIN_PHASE].ppsInst[0].push_back(instruction);
    // An array's second register must update its shared declaration as well.
    const string assembly = "dcl_resource_buffer                (" + kind + "," + kind + "," + kind + "," + kind + ") t" + (array ? "1" : "0") + "\n";
    d.ParseCode(&shader, assembly.c_str(), assembly.size());
    const string expected = "Buffer<" + kind + "4>";
    if (d.mErrorOccurred || d.mTextureType[0] != expected ||
        (array && d.mTextureType[1] != expected))
        throw std::runtime_error("Typed buffer metadata mismatch");
    const string output(d.mOutput.begin(), d.mOutput.end());
    const string name = reflectedType.empty() ? "t0" : "VS_t0";
    const string declaration = expected + " " + name + (array ? "[2]" : "") + " : register(t0);";
    if (output.find(declaration) == string::npos || output.find("StructuredBuffer") != string::npos ||
        d.mCodeStartPos != d.mOutput.size())
        throw std::runtime_error("Typed buffer declaration mismatch: " + output);
    return output + "\n" + kind + "4 main(uint index : TEXCOORD0) : SV_Target { return " + name + (array ? "[1]" : "") + ".Load(index); }\n";
}
