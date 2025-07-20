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
  
  buildPhase = ''
    gcc -fsanitize=address -fsanitize=undefined -g -O0 \
        `pkg-config --cflags gtk4` -Isrc/include \
        -o keepass-cloud-sync-debug \
        src/main.c src/ui/main_window.c src/ui/config_dialog.c \
        src/cloud/rclone.c src/cloud/config.c src/cloud/oauth.c \
        src/utils/file_utils.c src/utils/process_utils.c \
        `pkg-config --libs gtk4`
  '';
  
  installPhase = ''
    mkdir -p $out/bin
    cp keepass-cloud-sync-debug $out/bin/
  '';
}
