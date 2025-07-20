CC = gcc
CFLAGS = `pkg-config --cflags gtk4` -Isrc/include
LIBS = `pkg-config --libs gtk4`
TARGET = keepass-cloud-sync
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/ui/main_window.c $(SRCDIR)/ui/config_dialog.c $(SRCDIR)/cloud/rclone.c $(SRCDIR)/cloud/config.c $(SRCDIR)/cloud/oauth.c $(SRCDIR)/utils/file_utils.c $(SRCDIR)/utils/process_utils.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install