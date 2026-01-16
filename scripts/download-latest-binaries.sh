#!/usr/bin/env bash
set -e

echo "Downloading latest RTL binaries (Linux/Clang)..."

mkdir -p rtl-release-clang
cd rtl-release-clang

BASE_URL="https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP/releases/download/latest"

curl -L -o RTLTestRunApp \
  "$BASE_URL/RTLTestRunApp-clang"

curl -L -o RTLBenchmarkApp \
  "$BASE_URL/RTLBenchmarkApp-clang"
  
# Make executable
chmod +x RTLTestRunApp RTLBenchmarkApp

echo "Done downloading clang-prebuilt Test & Benchmarking binaries."
