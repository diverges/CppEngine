#!/bin/bash
# AIEngine Installation Script
# Cross-platform installation utility for development environment

set -e  # Exit on any error

# Configuration
INSTALL_PREFIX="${1:-/usr/local}"
ENGINE_VERSION="1.0.0"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENGINE_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on Windows (Git Bash/MSYS2)
is_windows() {
    [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || -n "$WINDIR" ]]
}

# Check dependencies
check_dependencies() {
    log_info "Checking build dependencies..."
    
    # Check compiler
    if ! command -v g++ &> /dev/null; then
        log_error "g++ compiler not found!"
        if is_windows; then
            log_info "Install via MSYS2: pacman -S mingw-w64-x86_64-gcc"
        else
            log_info "Install via package manager: sudo apt install build-essential"
        fi
        return 1
    fi
    log_success "C++ compiler found: $(g++ --version | head -n1)"
    
    # Check make
    if ! command -v make &> /dev/null; then
        log_error "make not found!"
        return 1
    fi
    log_success "Make found: $(make --version | head -n1)"
    
    # Check SDL2
    if is_windows; then
        if [ ! -d "/mingw64/include/SDL2" ]; then
            log_warning "SDL2 development headers not found"
            log_info "Install via MSYS2: pacman -S mingw-w64-x86_64-sdl2"
        else
            log_success "SDL2 development headers found"
        fi
    else
        if ! pkg-config --exists sdl2; then
            log_warning "SDL2 not found via pkg-config"
            log_info "Install via package manager: sudo apt install libsdl2-dev"
        else
            log_success "SDL2 found: $(pkg-config --modversion sdl2)"
        fi
    fi
    
    # Check GLEW
    if is_windows; then
        if [ ! -d "/mingw64/include/GL" ]; then
            log_warning "GLEW development headers not found"
            log_info "Install via MSYS2: pacman -S mingw-w64-x86_64-glew"
        else
            log_success "GLEW development headers found"
        fi
    else
        if ! pkg-config --exists glew; then
            log_warning "GLEW not found via pkg-config"
            log_info "Install via package manager: sudo apt install libglew-dev"
        else
            log_success "GLEW found: $(pkg-config --modversion glew)"
        fi
    fi
}

# Build engine
build_engine() {
    log_info "Building AIEngine library..."
    
    cd "$ENGINE_ROOT/engine"
    if ! make clean && make release; then
        log_error "Failed to build engine library"
        return 1
    fi
    
    if [ ! -f "lib/libAIEngine.a" ]; then
        log_error "Engine library not found after build"
        return 1
    fi
    
    log_success "Engine library built successfully"
}

# Install headers and library
install_engine() {
    log_info "Installing AIEngine to $INSTALL_PREFIX..."
    
    cd "$ENGINE_ROOT/engine"
    
    if is_windows; then
        # Windows installation
        mkdir -p "$INSTALL_PREFIX/include" "$INSTALL_PREFIX/lib" "$INSTALL_PREFIX/bin"
        
        # Copy headers
        cp -r "include/AIEngine" "$INSTALL_PREFIX/include/"
        log_success "Headers installed to $INSTALL_PREFIX/include/AIEngine"
        
        # Copy library
        cp "lib/libAIEngine.a" "$INSTALL_PREFIX/lib/"
        log_success "Library installed to $INSTALL_PREFIX/lib/libAIEngine.a"
        
        # Copy required DLLs if available
        if [ -d "/mingw64/bin" ]; then
            for dll in SDL2.dll glew32.dll; do
                if [ -f "/mingw64/bin/$dll" ]; then
                    cp "/mingw64/bin/$dll" "$INSTALL_PREFIX/bin/"
                    log_success "Copied $dll to $INSTALL_PREFIX/bin/"
                fi
            done
        fi
    else
        # Unix/Linux installation  
        sudo mkdir -p "$INSTALL_PREFIX/include" "$INSTALL_PREFIX/lib"
        
        # Copy headers
        sudo cp -r "include/AIEngine" "$INSTALL_PREFIX/include/"
        log_success "Headers installed to $INSTALL_PREFIX/include/AIEngine"
        
        # Copy library
        sudo cp "lib/libAIEngine.a" "$INSTALL_PREFIX/lib/"
        log_success "Library installed to $INSTALL_PREFIX/lib/libAIEngine.a"
        
        # Update library cache
        if command -v ldconfig &> /dev/null; then
            sudo ldconfig
            log_success "Library cache updated"
        fi
    fi
}

# Install pkg-config file
install_pkgconfig() {
    log_info "Creating pkg-config file..."
    
    local pkgconfig_dir
    if is_windows; then
        pkgconfig_dir="$INSTALL_PREFIX/lib/pkgconfig"
        mkdir -p "$pkgconfig_dir"
    else
        pkgconfig_dir="$INSTALL_PREFIX/lib/pkgconfig"
        sudo mkdir -p "$pkgconfig_dir"
    fi
    
    local pkgconfig_file="$pkgconfig_dir/aiengine.pc"
    
    cat > "/tmp/aiengine.pc" << EOF
prefix=$INSTALL_PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: AIEngine
Description: Modern C++ Game Engine
Version: $ENGINE_VERSION
Libs: -L\${libdir} -lAIEngine -lSDL2 -lGLEW -lGL
Cflags: -I\${includedir} -std=c++17
EOF
    
    if is_windows; then
        cp "/tmp/aiengine.pc" "$pkgconfig_file"
    else
        sudo cp "/tmp/aiengine.pc" "$pkgconfig_file"
    fi
    
    log_success "pkg-config file installed: $pkgconfig_file"
}

# Create development package
create_package() {
    log_info "Creating development package..."
    
    local package_name="aiengine-dev-$ENGINE_VERSION"
    local package_dir="/tmp/$package_name"
    
    rm -rf "$package_dir"
    mkdir -p "$package_dir"
    
    # Copy engine files
    cd "$ENGINE_ROOT"
    cp -r engine/include "$package_dir/"
    cp -r engine/lib "$package_dir/"
    cp -r engine/shaders "$package_dir/"
    cp -r docs "$package_dir/"
    
    # Copy examples and documentation
    cp README.md "$package_dir/"
    cp -r docs/examples "$package_dir/" 2>/dev/null || true
    
    # Create installation script for package
    cat > "$package_dir/install.sh" << 'EOF'
#!/bin/bash
INSTALL_PREFIX="${1:-/usr/local}"
echo "Installing AIEngine to $INSTALL_PREFIX..."
mkdir -p "$INSTALL_PREFIX/include" "$INSTALL_PREFIX/lib"
cp -r include/* "$INSTALL_PREFIX/include/"
cp -r lib/* "$INSTALL_PREFIX/lib/"
echo "Installation complete!"
EOF
    chmod +x "$package_dir/install.sh"
    
    # Create archive
    cd "/tmp"
    tar -czf "$package_name.tar.gz" "$package_name"
    
    log_success "Development package created: /tmp/$package_name.tar.gz"
    log_info "Extract and run: tar -xzf $package_name.tar.gz && cd $package_name && ./install.sh"
}

# Test installation
test_installation() {
    log_info "Testing installation..."
    
    # Test compile simple program
    cat > "/tmp/test_aiengine.cpp" << 'EOF'
#include <AIEngine/AIEngine.hpp>
#include <iostream>

int main() {
    std::cout << "AIEngine headers accessible!" << std::endl;
    return 0;
}
EOF
    
    local compile_cmd
    if is_windows; then
        compile_cmd="g++ -std=c++17 -I$INSTALL_PREFIX/include /tmp/test_aiengine.cpp -L$INSTALL_PREFIX/lib -lAIEngine -o /tmp/test_aiengine"
    else
        compile_cmd="g++ -std=c++17 -I$INSTALL_PREFIX/include /tmp/test_aiengine.cpp -L$INSTALL_PREFIX/lib -lAIEngine -o /tmp/test_aiengine"
    fi
    
    if eval "$compile_cmd" 2>/dev/null; then
        log_success "Test compilation successful!"
        rm -f /tmp/test_aiengine /tmp/test_aiengine.cpp
    else
        log_warning "Test compilation failed - check library paths"
    fi
}

# Main installation function
install_aiengine() {
    log_info "=== AIEngine Installation Script ==="
    log_info "Version: $ENGINE_VERSION"
    log_info "Install prefix: $INSTALL_PREFIX"
    log_info "Engine root: $ENGINE_ROOT"
    echo
    
    # Run installation steps
    check_dependencies || { log_error "Dependency check failed"; exit 1; }
    echo
    
    build_engine || { log_error "Build failed"; exit 1; }
    echo
    
    install_engine || { log_error "Installation failed"; exit 1; }
    echo
    
    install_pkgconfig || { log_warning "pkg-config installation failed"; }
    echo
    
    test_installation
    echo
    
    log_success "=== AIEngine installation complete! ==="
    echo
    log_info "Usage in your projects:"
    log_info "  Compile: g++ -std=c++17 \$(pkg-config --cflags aiengine) your_game.cpp \$(pkg-config --libs aiengine)"
    log_info "  Headers: #include <AIEngine/AIEngine.hpp>"
    echo
    log_info "Documentation: $ENGINE_ROOT/docs/"
    log_info "Examples: $ENGINE_ROOT/docs/examples/"
}

# Uninstall function
uninstall_aiengine() {
    log_info "Uninstalling AIEngine from $INSTALL_PREFIX..."
    
    if is_windows; then
        rm -rf "$INSTALL_PREFIX/include/AIEngine"
        rm -f "$INSTALL_PREFIX/lib/libAIEngine.a"
        rm -f "$INSTALL_PREFIX/lib/pkgconfig/aiengine.pc"
    else
        sudo rm -rf "$INSTALL_PREFIX/include/AIEngine"
        sudo rm -f "$INSTALL_PREFIX/lib/libAIEngine.a"
        sudo rm -f "$INSTALL_PREFIX/lib/pkgconfig/aiengine.pc"
    fi
    
    log_success "AIEngine uninstalled"
}

# Command line interface
case "${1:-install}" in
    "install")
        shift
        INSTALL_PREFIX="${1:-$INSTALL_PREFIX}"
        install_aiengine
        ;;
    "uninstall")
        shift
        INSTALL_PREFIX="${1:-$INSTALL_PREFIX}"
        uninstall_aiengine
        ;;
    "package")
        create_package
        ;;
    "check")
        check_dependencies
        ;;
    "help"|"--help"|"-h")
        echo "AIEngine Installation Script"
        echo ""
        echo "Usage: $0 [command] [install_prefix]"
        echo ""
        echo "Commands:"
        echo "  install [prefix]   Install AIEngine (default: /usr/local)"
        echo "  uninstall [prefix] Uninstall AIEngine"
        echo "  package            Create development package"
        echo "  check              Check build dependencies"
        echo "  help               Show this help"
        echo ""
        echo "Examples:"
        echo "  $0 install                    # Install to /usr/local"
        echo "  $0 install /opt/aiengine      # Install to custom location"
        echo "  $0 uninstall                  # Remove from /usr/local"
        echo "  $0 package                    # Create portable package"
        ;;
    *)
        log_error "Unknown command: $1"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac