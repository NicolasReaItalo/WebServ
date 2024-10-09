#!/bin/bash

# Define variables
HURL_VERSION="3.0.0"  # Replace with the latest version number
HURL_URL="https://github.com/Orange-OpenSource/hurl/releases/download/${HURL_VERSION}/hurl-${HURL_VERSION}-x86_64-linux.tar.gz"
INSTALL_DIR="$HOME/bin"
TAR_FILE="hurl-${HURL_VERSION}-x86_64-linux.tar.gz"
EXTRACTED_DIR="hurl-${HURL_VERSION}-x86_64-linux"

# Create ~/bin directory if it doesn't exist
mkdir -p "$INSTALL_DIR"

# Download Hurl binary
echo "Downloading Hurl ${HURL_VERSION}..."
wget -q "$HURL_URL" -O "$TAR_FILE"

if [ $? -ne 0 ]; then
    echo "Failed to download Hurl. Please check the version and your internet connection."
    exit 1
fi

# Extract the tar.gz file
echo "Extracting Hurl..."
tar -xzf "$TAR_FILE"

# Move the binary to ~/bin
echo "Installing Hurl to $INSTALL_DIR..."
mv "$EXTRACTED_DIR/hurl" "$INSTALL_DIR/"

# Cleanup
rm -rf "$TAR_FILE" "$EXTRACTED_DIR"

# Add ~/bin to PATH in .bashrc if not already added
if ! grep -q "$INSTALL_DIR" ~/.bashrc; then
    echo "Adding $INSTALL_DIR to PATH in .bashrc..."
    echo "export PATH=\$HOME/bin:\$PATH" >> ~/.bashrc
fi

# Source the .bashrc to apply the changes immediately
echo "Reloading shell configuration..."
source ~/.bashrc

# Check if Hurl is installed correctly
if command -v hurl &> /dev/null; then
    echo "Hurl installed successfully! Version:"
    hurl --version
else
    echo "Hurl installation failed."
fi

