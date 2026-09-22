@echo off
setlocal
cd /d "%~dp0..\.."
if not exist HLSLDecompiler\tests\out mkdir HLSLDecompiler\tests\out
cl /nologo /EHsc /std:c++14 /D_CRT_SECURE_NO_WARNINGS /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 /I. /IBinaryDecompiler/include /IBinaryDecompiler HLSLDecompiler\tests\typed_buffer.cpp HLSLDecompiler\tests\emit_typed_buffer.cpp BinaryDecompiler\decode.cpp BinaryDecompiler\decodeDX9.cpp BinaryDecompiler\reflect.cpp /Fo:HLSLDecompiler\tests\out\ /Fe:HLSLDecompiler\tests\out\typed_buffer.exe /link d3d11.lib d3dcompiler.lib
if errorlevel 1 exit /b 1
HLSLDecompiler\tests\out\typed_buffer.exe
exit /b %errorlevel%
