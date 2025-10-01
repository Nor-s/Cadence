export PATH="/opt/homebrew/bin:$PATH"

BUILD_TYPE=${1:-debug}

meson setup build/wasm --buildtype=$BUILD_TYPE --cross-file wasm.ini --default-library=static -Dplatform=web

meson compile -C build/wasm