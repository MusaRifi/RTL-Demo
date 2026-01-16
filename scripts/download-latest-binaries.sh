#!/usr/bin/env bash
set -e

echo "Downloading latest RTL binaries (Linux/clang)..."

mkdir -p prebuilt-bins
cd prebuilt-bins

BASE_URL="https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP/releases/download/latest"

curl -L -o RTLTestRunApp-linux-clang \
  "$BASE_URL/RTLTestRunApp-linux-clang"

curl -L -o RTLBenchmarkApp-linux-clang \
  "$BASE_URL/RTLBenchmarkApp-linux-clang"

chmod +x RTLTestRunApp-linux-clang RTLBenchmarkApp-linux-clang

echo "Done."