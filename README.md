# KeePass Cloud Sync

A simple GTK4 application for syncing KeePass database files (.kdbx) to popular cloud providers using rclone.

## Features

- Select KeePass database files (.kdbx)
- Upload to cloud providers (Google Drive, Dropbox, OneDrive, Box, pCloud)
- Download from cloud providers
- Simple GTK4 interface

## Prerequisites

- GTK4 development libraries
- rclone configured with your cloud providers
- GCC compiler

### Install dependencies on Ubuntu/Debian:
```bash
sudo apt install libgtk-4-dev build-essential
```

### Install dependencies on Fedora:
```bash
sudo dnf install gtk4-devel gcc make
```

### Install rclone:
```bash
curl https://rclone.org/install.sh | sudo bash
```

## Setup

1. Configure rclone with your cloud providers:
```bash
rclone config
```

2. Build the application:
```bash
make
```

3. Run the application:
```bash
./keepass-cloud-sync
```

## Usage

1. Click "Select File" to choose your KeePass database (.kdbx file)
2. Select your cloud provider from the dropdown
3. Enter the remote path (default: /KeePass/)
4. Click "Upload to Cloud" to sync your database to the cloud
5. Click "Download from Cloud" to download the latest version

## Cloud Provider Setup

Make sure to configure rclone for your chosen cloud provider:

- **Google Drive**: `rclone config` → Choose "gdrive"
- **Dropbox**: `rclone config` → Choose "dropbox"  
- **OneDrive**: `rclone config` → Choose "onedrive"
- **Box**: `rclone config` → Choose "box"
- **pCloud**: `rclone config` → Choose "pcloud"

## Security Notes

- This application uses rclone to handle cloud authentication
- Your KeePass database remains encrypted during transfer
- Make sure your rclone configuration is properly secured
- Consider using rclone's built-in encryption for additional security