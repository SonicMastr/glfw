echo -e "\033[1;32mGLFW3 Vita Port by \033[0;31mSonicMastr\033[0m"
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$DOLCESDK/share/dolce.toolchain.cmake -Bbuild -G "Unix Makefiles"
cd build
make -j4
cp src/libglfw3.a $DOLCESDK/arm-dolce-eabi/lib/
echo -e "\033[1;32mCopied libglfw3.a"
cp -r ../include/GLFW $DOLCESDK/arm-dolce-eabi/include/
echo "Copied GLFW Headers"