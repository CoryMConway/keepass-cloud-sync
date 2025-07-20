{ pkgs, ... }:

{
  packages = with pkgs; [
    # Build tools
    gcc
    gnumake
    pkg-config
    wrapGAppsHook4
    
    # GTK4 development
    gtk4
    glib
    glib-networking
    gsettings-desktop-schemas
    
    # Runtime dependencies
    rclone
    
    # Development tools
    gdb
    valgrind
  ];

  env = {
    # Ensure pkg-config can find GTK4
    PKG_CONFIG_PATH = "${pkgs.gtk4.dev}/lib/pkgconfig:${pkgs.glib.dev}/lib/pkgconfig";
  };

  scripts = {
    build-debug = {
      exec = ''
        echo "Building debug version with AddressSanitizer..."
        
        # Create debug build with wrapping
        cat > build-debug.nix << 'EOF'
{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "keepass-cloud-sync-debug";
  src = ./.;
  
  nativeBuildInputs = with pkgs; [ 
    gcc 
    pkg-config 
    wrapGAppsHook4
  ];
  
  buildInputs = with pkgs; [ 
    gtk4 
    glib 
    glib-networking 
    gsettings-desktop-schemas
  ];
  
  buildPhase = '''
    gcc -fsanitize=address -fsanitize=undefined -g -O0 \
        `pkg-config --cflags gtk4` -Isrc/include \
        -o keepass-cloud-sync-debug \
        src/main.c src/ui/main_window.c src/ui/config_dialog.c \
        src/cloud/rclone.c src/cloud/config.c src/cloud/oauth.c \
        src/utils/file_utils.c src/utils/process_utils.c \
        `pkg-config --libs gtk4`
  ''';
  
  installPhase = '''
    mkdir -p $out/bin
    cp keepass-cloud-sync-debug $out/bin/
  ''';
}
EOF
        
        echo "Building debug version with proper GTK wrapping..."
        nix-build build-debug.nix -o result-debug
        echo "Debug build complete: ./result-debug/bin/keepass-cloud-sync-debug"
      '';
      description = "Build debug version with sanitizers and GTK wrapping";
    };

    build-release = {
      exec = ''
        echo "Building release version with proper GTK wrapping..."
        
        # Create release build with wrapping
        cat > build-release.nix << 'EOF'
{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "keepass-cloud-sync";
  src = ./.;
  
  nativeBuildInputs = with pkgs; [ 
    gcc 
    pkg-config 
    wrapGAppsHook4
  ];
  
  buildInputs = with pkgs; [ 
    gtk4 
    glib 
    glib-networking 
    gsettings-desktop-schemas
  ];
  
  buildPhase = '''
    gcc -O2 -DNDEBUG \
        `pkg-config --cflags gtk4` -Isrc/include \
        -o keepass-cloud-sync \
        src/main.c src/ui/main_window.c src/ui/config_dialog.c \
        src/cloud/rclone.c src/cloud/config.c src/cloud/oauth.c \
        src/utils/file_utils.c src/utils/process_utils.c \
        `pkg-config --libs gtk4`
  ''';
  
  installPhase = '''
    mkdir -p $out/bin
    cp keepass-cloud-sync $out/bin/
  ''';
}
EOF
        
        echo "Building release version with proper GTK wrapping..."
        nix-build build-release.nix -o result-release
        echo "Release build complete: ./result-release/bin/keepass-cloud-sync"
      '';
      description = "Build release version with GTK wrapping";
    };

    run-debug = {
      exec = ''
        echo "Running debug version..."
        ./result-debug/bin/keepass-cloud-sync-debug
      '';
      description = "Run debug version";
    };

    run-release = {
      exec = ''
        echo "Running release version..."
        ./result-release/bin/keepass-cloud-sync
      '';
      description = "Run release version";
    };
  };

  enterShell = ''
    echo "KeePass Cloud Sync Development Environment"
    echo "========================================="
    echo "Available commands:"
    echo "  build-debug   - Build debug version with AddressSanitizer & GTK wrapping"
    echo "  build-release - Build release version with GTK wrapping"
    echo "  run-debug     - Run debug version"
    echo "  run-release   - Run release version"
    echo "  make          - Build using Makefile (basic, no wrapping)"
    echo "  make clean    - Clean build artifacts"
    echo ""
    echo "Workflow:"
    echo "  1. build-release  - Build production version"
    echo "  2. run-release    - Run production version"
    echo "  3. build-debug    - Build debug version (for development)"
    echo "  4. run-debug      - Run debug version (for development)"
    echo ""
    echo "GTK4 version: $(pkg-config --modversion gtk4)"
    echo "RClone version: $(rclone version --check=false | head -1)"
    echo ""
    echo "Cloud providers can be configured directly in the GUI!"
  '';
}