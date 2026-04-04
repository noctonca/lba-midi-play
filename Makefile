CC      = cc
CFLAGS  = -std=c99 -O2 -Wall -Wextra
TARGET  = lba-midi-play
SRCS    = main.c hqr.c xmidi.c

# Platform-specific linker flags
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    LDFLAGS = -framework CoreAudio -framework AudioToolbox \
              -framework CoreFoundation -lpthread
else
    LDFLAGS = -lpthread -ldl -lm
endif

$(TARGET): $(SRCS) hqr.h xmidi.h tsf.h tml.h miniaudio.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
