call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
cd ..
mkdir tmp_build
cd Release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.6.2\msvc2019_64" -G "NMake Makefiles" ..
nmake
cpack -G ZIP
xcopy /y /E packages/ ../

cd ..