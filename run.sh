
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/Users/eevv/Qt/6.10.2/macos"
cmake --build build -j8
./build/MachOExplorer