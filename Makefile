SRCDIR        := src
SRCINCLUDE    := $(SRCDIR)/include
OBJDIR        := obj
LIBDIR        := lib
EXTERNDIR     := extern
EXTERNINCLUDE := $(EXTERNDIR)/include
EXTERNLIB     := $(EXTERNDIR)/lib
BINDIR        := bin

ifeq ($(OS), Windows_NT)
	GCC_LINUX   := gcc
	GCC_WINDOWS := desdewindowsnopuedes
else
	GCC_LINUX   := $(shell which gcc)
	GCC_WINDOWS := $(shell which x86_64-w64-mingw32-gcc)
endif

STDC               := gnu11
OPTIMIZATION_LEVEL := 3
RUNTIME_DIAGS      := true

CFLAGS := \
	-std=$(STDC) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fstrict-aliasing -fmax-errors=0\
	-D_GNU_SOURCE -static --static -L $(LIBDIR) -I $(EXTERNINCLUDE) -include $(SRCINCLUDE)/trivia.h
ifeq ($(RUNTIME_DIAGS), false)
	CFLAGS += -DDISABLE_RUNTIME_DIAGS
endif

DFLAGS := \
	-g3 -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow -Wstrict-prototypes -Wwrite-strings -Wconversion -Wcast-align

LIBS := -pthread

linux:      CC := $(GCC_LINUX)
ui_linux:   CC := $(GCC_LINUX)
os_linux:   CC := $(GCC_LINUX)

windows:    CC     := $(GCC_WINDOWS)
# windows:    CFLAGS += -mwindows
ui_windows: CC     := $(GCC_WINDOWS)
# ui_windows: CFLAGS += -mwindows
os_windows: CC     := $(GCC_WINDOWS)
# os_windows: CFLAGS += -mwindows

ifeq ($(OS), Windows_NT)
all: windows
else
all: linux windows
endif

init: init_bin init_lib init_obj

init_bin:
	@echo $(shell mkdir $(BINDIR))
init_lib:
	@echo $(shell mkdir $(LIBDIR))
init_obj:
ifeq ($(OS), Windows_NT)
	@echo $(shell mkdir $(OBJDIR))
else
	@echo $(shell mkdir -p $(OBJDIR))
endif

UILINUX := $(LIBDIR)/libuilinux.a
UILINUXOBJS := $(patsubst $(SRCDIR)/ui/%.c, $(OBJDIR)/ui_linux_%.o, $(wildcard $(SRCDIR)/ui/*.c))
OSLINUX := $(LIBDIR)/liboslinux.a
OSLINUXOBJS := $(patsubst $(SRCDIR)/os/%.c, $(OBJDIR)/os_linux_%.o, $(wildcard $(SRCDIR)/os/*.c))
OSLINUXOBJS += $(patsubst $(SRCDIR)/os/linux/%.c, $(OBJDIR)/os_linux_linux_%.o, $(wildcard $(SRCDIR)/os/linux/*.c))

linux: ui_linux os_linux

ui_linux: init $(UILINUX)

$(UILINUX): $(UILINUXOBJS)
	ar rcs $@ $^

$(OBJDIR)/ui_linux_%.o: $(SRCDIR)/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

os_linux: init $(OSLINUX)

$(OSLINUX): $(OSLINUXOBJS)
	ar rcs $@ $^

$(OBJDIR)/os_linux_%.o: $(SRCDIR)/os/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/os_linux_linux_%.o: $(SRCDIR)/os/linux/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

UIWINDOWS := $(LIBDIR)/libuiwindows.a
UIWINDOWSOBJS := $(patsubst $(SRCDIR)/ui/%.c, $(OBJDIR)/ui_windows_%.o, $(wildcard $(SRCDIR)/ui/*.c))
OSWINDOWS := $(LIBDIR)/liboswindows.a
OSWINDOWSOBJS := $(patsubst $(SRCDIR)/os/%.c, $(OBJDIR)/os_windows_%.o, $(wildcard $(SRCDIR)/os/*.c))
OSWINDOWSOBJS += $(patsubst $(SRCDIR)/os/windows/%.c, $(OBJDIR)/os_windows_windows_%.o, $(wildcard $(SRCDIR)/os/windows/*.c))

windows: ui_windows os_windows

ui_windows: init $(UIWINDOWS)

$(UIWINDOWS): $(UIWINDOWSOBJS)
	ar rcs $@ $^

$(OBJDIR)/ui_windows_%.o: $(SRCDIR)/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

os_windows: init $(OSWINDOWS)

$(OSWINDOWS): $(OSWINDOWSOBJS)
	ar rcs $@ $^

$(OBJDIR)/os_windows_%.o: $(SRCDIR)/os/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/os_windows_windows_%.o: $(SRCDIR)/os/windows/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	@echo $(shell rmdir /s /q $(BINDIR) $(LIBDIR) $(OBJDIR))
else
	@echo $(shell rm -rf $(BINDIR) $(LIBDIR) $(OBJDIR))
endif

PHONY += clean
.PHONY: $(PHONY)

.SILENT: init_bin init_obj init_lib