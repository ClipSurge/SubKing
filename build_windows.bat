@echo off
REM Vaatii: Visual Studio 2022 (Desktop C++), CMake, Git
cmake -B build -G "Visual Studio 17 2022" -A x64 || exit /b 1
cmake --build build --config Release --parallel || exit /b 1
echo.
echo Valmis: build\SubKing_artefacts\Release\VST3\SubKing.vst3
echo Kopioi se kansioon C:\Program Files\Common Files\VST3\  (admin-oikeudet)
pause
