"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" x86
cd ..
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" ..