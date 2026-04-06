CC      = cc
CFLAGS  = -std=c99 -O2 -Wall -Wextra
TARGET  = lba-midi-play
SRCS    = main.c hqr.c xmidi.c

# Platform-specific linker flags
# MSYS2 MinGW: uname is like MINGW64_NT-10.0 — no -ldl (not used like on Linux).
UNAME_S := $(shell uname -s 2>/dev/null || echo unknown)

ifeq ($(UNAME_S), Darwin)
    LDFLAGS = -framework CoreAudio -framework AudioToolbox \
              -framework CoreFoundation -lpthread
else ifneq ($(findstring MINGW,$(UNAME_S)),)
    LDFLAGS = -lm -lpthread
else ifneq ($(findstring MSYS,$(UNAME_S)),)
    LDFLAGS = -lm -lpthread
else
    LDFLAGS = -lpthread -ldl -lm
endif

$(TARGET): $(SRCS) hqr.h xmidi.h tsf.h tml.h miniaudio.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: clean
