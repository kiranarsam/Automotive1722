#!/bin/bash

set +ex

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="$SCRIPT_DIR"/build

cmake -E make_directory "$BUILD_DIR"

cd "$BUILD_DIR" || exit

cmake "$SCRIPT_DIR" -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/install"

sleep 0.5
cmake --build . -j "$(nproc)"
if [ $? -ne 0 ]; then
    exit 1
fi
cmake --install .

echo
echo "### Build done"
echo