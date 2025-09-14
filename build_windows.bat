@REM meson setup --cross-file w64-mingw32.txt build/native --buildtype=debug --default-library=static -Dplatform=native
meson setup  --backend vs2022 build/win/debug --buildtype=debug  -Dplatform=native --default-library=static
@REM meson compile -C build/native