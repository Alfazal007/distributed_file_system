#!/bin/bash

set -e

echo "🧹 Cleaning old builds..."
rm -rf build/ storageLayer data

echo "🔨 Building project..."
make

echo "🚀 Running storageLayer..."
./storageLayer
