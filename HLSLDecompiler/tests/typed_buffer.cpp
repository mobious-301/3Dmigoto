#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <iostream>
#include <stdexcept>
#include <string>

using Microsoft::WRL::ComPtr;
using std::string;
string emit_typed_buffer(const string &, const string &, bool);

int main()
{
    try {
        int count = 0;
        for (const string kind : {string("float"), string("uint")}) {
            for (const string reflected : {string(), string("StructuredBuffer<float>"),
                string("Buffer<float>"), "Buffer<" + kind + "4>"}) {
                for (bool array : {false, true}) {
                    if (array && reflected.empty()) continue;
                    const string source = emit_typed_buffer(kind, reflected, array);
                    ComPtr<ID3DBlob> bytecode, errors, assembly;
                    if (FAILED(D3DCompile(source.data(), source.size(), nullptr, nullptr, nullptr,
                        "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &bytecode, &errors)))
                        throw std::runtime_error(errors ? (const char *)errors->GetBufferPointer() : "Compile failed");
                    if (FAILED(D3DDisassemble(bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
                        0, nullptr, &assembly))) throw std::runtime_error("Disassembly failed");
                    const string dxbc((const char *)assembly->GetBufferPointer(), assembly->GetBufferSize());
                    const string expected = "dcl_resource_buffer (" + kind + "," + kind + "," + kind + "," + kind + ") t" + (array ? "1" : "0");
                    if (dxbc.find(expected) == string::npos || dxbc.find("dcl_resource_structured") != string::npos)
                        throw std::runtime_error("DXBC resource mismatch: " + dxbc);
                    ++count;
                }
            }
        }
        std::cout << "PASS: " << count << " typed buffer declaration/FXC round-trip cases\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
