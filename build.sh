#!/bin/bash

set +ex

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_TYPE=""
BUILD_TARGET=""

while [[ "$#" -gt 0 ]]; do
    case "$1" in
    "debug" | "release")
        BUILD_TYPE="$1"
        shift 1
        ;;
    "aarch64" | "x86_64")
        BUILD_TARGET="$1"
        shift 1
        ;;
    *)
        shift 1
        ;;
    esac
done

[ -z "$BUILD_TYPE" ] && BUILD_TYPE="debug"
[ -z "$BUILD_TARGET" ] && BUILD_TARGET="x86_64"

BUILD_DIR="$SCRIPT_DIR/build/$BUILD_TARGET/$BUILD_TYPE"
SYSTEM_PROCESSOR=$(uname -p)

cmake -E make_directory "$BUILD_DIR"

cd "$BUILD_DIR" || exit

cmake "$SCRIPT_DIR" -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/install" -DCMAKE_SYSTEM_PROCESSOR="$SYSTEM_PROCESSOR" -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/target/$BUILD_TARGET.toolchain"

cmake --build . -j "$(nproc)"
if [ $? -ne 0 ]; then
    exit 1
fi
cmake --install .

echo "Done"
