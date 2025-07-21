# KeePass Cloud Sync

A modular GTK4 application for syncing KeePass database files (.kdbx) to popular cloud providers using rclone. Features built-in OAuth configuration for cloud providers.

## Features

- **File Management**: Select KeePass database files (.kdbx) with file picker
- **Cloud Providers**: Support for Google Drive, Dropbox, OneDrive, Box, pCloud
- **Built-in OAuth**: Configure cloud providers directly in the app (no manual rclone setup needed)
- **Upload/Download**: Sync files to and from cloud storage
- **Modern UI**: Clean GTK4 interface with provider-specific setup instructions
- **Modular Code**: Well-organized C codebase following modern software architecture

## Project Structure

| Folder | Purpose | Contents |
|--------|---------|----------|
| `src/` | Main source code | Entry point and core application |
| `src/include/` | Header files | Function declarations and data structures |
| `src/ui/` | User interface | GTK4 windows, dialogs, and UI components |
| `src/cloud/` | Cloud operations | rclone commands, OAuth flows, config management |
| `src/utils/` | Utilities | File operations, process management, shared helpers |
| `schemas/` | Configuration | Application schemas and templates |

## Prerequisites

### Required Dependencies
- **GTK4**: UI framework (`libgtk-4-dev`)
- **GCC**: C compiler
- **pkg-config**: Build configuration
- **rclone**: Cloud storage backend
- **curl**: For OAuth token exchange (usually pre-installed)

### Install dependencies on Ubuntu/Debian:
```bash
sudo apt install libgtk-4-dev build-essential pkg-config curl
```

### Install dependencies on Fedora:
```bash
sudo dnf install gtk4-devel gcc make pkg-config curl
```

### Install dependencies on Arch:
```bash
sudo pacman -S gtk4 gcc make pkg-config curl
```

### Install rclone:
```bash
curl https://rclone.org/install.sh | sudo bash
```

## Building

> **Development Note**: This project was developed on NixOS using Nix for reproducible builds and dependency management. The Nix builds include proper GTK wrapping and are the recommended way to build.

### Option 1: Nix Build (Recommended - works on any Linux)
**Production build with GTK environment wrapping:**
```bash
nix-build build-wrapped.nix
./result/bin/keepass-cloud-sync
```

**Debug build with AddressSanitizer and debugging symbols:**
```bash
nix-build build-debug.nix
./result-debug/bin/keepass-cloud-sync-debug
```

**Development environment (NixOS users):**
```bash
nix develop
build-release    # Build production version  
build-debug      # Build debug version
run-release      # Run production version
run-debug        # Run debug version
```

### Option 2: Standard Build (Non-NixOS Linux distributions)
**For Ubuntu/Debian/Fedora/Arch users without Nix:**

1. **Install Nix package manager** (recommended):
   ```bash
   curl -L https://nixos.org/nix/install | sh
   . ~/.nix-profile/etc/profile.d/nix.sh
   ```
   Then use Nix builds above for best compatibility.

2. **OR use system packages** (may have GTK environment issues):
   ```bash
   make
   ./keepass-cloud-sync
   ```
   
   **Note**: The Makefile build may have GTK theming/icon issues compared to the Nix build which includes proper `wrapGAppsHook4` integration.

## Usage

### Getting Started
1. **Launch the application**:
   ```bash
   ./keepass-cloud-sync
   # or if using Nix:
   ./result/bin/keepass-cloud-sync
   ```

2. **Configure Cloud Provider** (First time only):
   - Click "Configure Cloud Providers"
   - Select your provider (Google Drive, Dropbox, etc.)
   - Click "Open [Provider] Console" to get API credentials
   - For Google Drive: Follow the detailed step-by-step instructions
   - Enter Client ID and Client Secret
   - Complete OAuth flow

3. **Sync Files**:
   - Click "Select File" to choose your KeePass database (.kdbx)
   - Select configured cloud provider from dropdown
   - Set remote path (default: `/KeePass/`)
   - **Upload**: Click "Upload to Cloud"
   - **Download**: Click "Download from Cloud" (saves to home directory)

### Cloud Provider Setup

The app includes **built-in OAuth configuration** - no manual rclone setup required!

**Supported Providers:**
- ✅ **Google Drive**: Full OAuth integration with step-by-step setup guide
- 🔄 **Dropbox**: Untested
- 🔄 **OneDrive**: Untested
- 🔄 **Box**: Untested
- 🔄 **pCloud**: Untested

**Manual rclone setup** (alternative):
```bash
rclone config
```

## Development

### Development Environment
This project was developed on **NixOS** with a reproducible development environment:

- **NixOS Development**: Uses `devenv.nix` for consistent dependency management
- **Cross-platform**: Nix builds work on any Linux distribution
- **GTK Integration**: `wrapGAppsHook4` ensures proper GTK theming and icons
- **Debug Support**: `build-debug.nix` includes AddressSanitizer and debugging symbols

### Code Architecture
- **Modular Design**: Separated UI, cloud operations, and utilities
- **Header-based**: Clean interfaces between modules  
- **Memory Safe**: Proper GLib memory management
- **Error Handling**: Comprehensive error checking and user feedback

### Building from Source
**Recommended (works on any Linux):**
```bash
git clone https://github.com/CoryMConway/keepass-cloud-sync.git
cd keepass-cloud-sync
nix-build build-wrapped.nix
```

**Alternative (system dependencies):**
```bash
git clone https://github.com/CoryMConway/keepass-cloud-sync.git
cd keepass-cloud-sync
make
```

### Why Nix?
- **Reproducible builds**: Same environment on any machine
- **GTK wrapping**: Proper desktop integration and theming
- **Development tools**: Integrated debugging and sanitizers

### File Organization
```
src/
├── main.c                    # Application entry point (14 lines)
├── include/
│   ├── app_data.h           # Core data structures
│   ├── ui.h                 # UI function declarations  
│   ├── cloud.h              # Cloud provider interfaces
│   └── utils.h              # Utility function declarations
├── ui/
│   ├── main_window.c        # Main application window
│   └── config_dialog.c      # Provider configuration dialog
├── cloud/
│   ├── rclone.c            # rclone command execution
│   ├── config.c            # Configuration file management
│   └── oauth.c             # OAuth authentication flows
└── utils/
    ├── file_utils.c        # File and path operations
    └── process_utils.c     # Process execution and URL handling
```

## Security Notes

- **Encrypted Transfer**: KeePass databases remain encrypted during cloud sync
- **OAuth Security**: Uses industry-standard OAuth 2.0 flows
- **Local Storage**: OAuth tokens stored securely in rclone config
- **No Passwords**: App never stores or transmits your KeePass master password
- **Open Source**: Full source code available for security review

## Troubleshooting

### Build Issues
- Ensure GTK4 development packages are installed
- Check that `pkg-config` can find GTK4: `pkg-config --modversion gtk4`

### OAuth Issues
- **Google Drive "access blocked"**: Add your email as a test user in Google Cloud Console
- **Missing credentials**: Make sure to save Client ID/Secret before starting OAuth
- **Token expiry**: Re-run OAuth setup if sync fails

### File Syncing
- You can publish from any directory, but syncs go to your home directory (`~/`)
- Check for `.kdbx` files: `ls -la ~/*.kdbx`

## License

Open source - feel free to contribute or fork for your own use.