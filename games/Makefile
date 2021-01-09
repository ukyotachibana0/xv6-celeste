GAME ?= celeste

LD = $(CC)
RM = rm -f

CFLAGS += -I. -g

# macOS
# LDEXTRAFLAGS = raylib/lib/libraylib_macos.a -framework OpenGL -framework OpenAL -framework IOKit -framework CoreVideo -framework Cocoa

# Linux
LDEXTRAFLAGS = raylib/lib/libraylib_linux.a -lc -lm -ldl -lpthread

OBJS = main.o $(patsubst %.c, %.o, $(wildcard $(GAME)/*.c))

game: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDEXTRAFLAGS)

clean:
	$(RM) game $(OBJS)

.PHONY: clean
