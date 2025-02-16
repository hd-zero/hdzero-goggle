#!/bin/bash

SOURCE_DIR="./out"
TARGET_DIR="../../lib/lvgl/lvgl/src/font"

echo "Copying fonts..."
cp "$SOURCE_DIR"/*.c "$TARGET_DIR"
if [ $? -eq 0 ]; then
    echo "Font files copied successfully."
else
    echo "Error while copying files."
fi
