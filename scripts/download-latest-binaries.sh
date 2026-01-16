#!/usr/bin/env bash
set -e

echo "Downloading latest RTL binaries (Linux/Clang)..."

mkdir -p rtl-release-clang
cd rtl-release-clang

BASE_URL="https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP/releases/download/latest"

curl -L -o RTLTestRunApp-linux-clang \
  "$BASE_URL/RTLTestRunApp-clang"

curl -L -o RTLBenchmarkApp-clang \
  "$BASE_URL/RTLBenchmarkApp-clang"

# Rename to clean names
mv RTLTestRunApp-clang RTLTestRunApp
mv RTLBenchmarkApp-clang RTLBenchmarkApp

# Make executable
chmod +x RTLTestRunApp RTLBenchmarkApp

echo "Done downloading clang-prebuilt Test & Benchmarking binaries."
