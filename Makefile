SRCDIR        := src
SRCINCLUDE    := $(SRCDIR)/include
OBJDIR        := obj
LIBDIR        := lib
EXTERNDIR     := extern
EXTERNINCLUDE := $(EXTERNDIR)/include
EXTERNLIB     := $(EXTERNDIR)/lib
BINDIR        := bin

ifeq ($(OS), Windows_NT)
	GCC_LINUX   := desdewindowsnopuedes
	GCC_WINDOWS := gcc
else
	GCC_LINUX   := $(shell which gcc)
	GCC_WINDOWS := $(shell which x86_64-w64-mingw32-gcc)
endif

STDC               := gnu11
OPTIMIZATION_LEVEL := 3
RUNTIME_DIAGS      := true

CFLAGS := \
	-std=$(STDC) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fstrict-aliasing -fmax-errors=1 \
	-D_GNU_SOURCE -I $(EXTERNINCLUDE) -static --static -static-libgcc -include $(SRCINCLUDE)/trivia.h -pthread -lpthread
ifeq ($(RUNTIME_DIAGS), false)
	CFLAGS += -DDISABLE_RUNTIME_DIAGS
endif

DFLAGS := \
	-g3 -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow=local -Wstrict-prototypes -Wwrite-strings -Wconversion -Wcast-align

UILIBS     := -lformw -lmenuw -lpanelw -lncursesw
OSLIBS     := -lbacktrace
BDLIBS     := -lsqlite3
TRIVIALIBS := -lui -los -lserver -lbd
LIBS       =  $(TRIVIALIBS) $(UILIBS) $(OSLIBS)

ifeq ($(OS), Windows_NT)
all: windows
else
all: linux windows
endif

linux: CC     := $(GCC_LINUX)
linux: LIBDIR := $(LIBDIR)/linux
linux: CFLAGS += -I $(EXTERNINCLUDE)/linux -L $(EXTERNLIB)/linux -L $(LIBDIR)
linux: OBJDIR := $(OBJDIR)/linux
linux: BINDIR := $(BINDIR)/linux

windows: CC     := $(GCC_WINDOWS)
windows: LIBDIR := $(LIBDIR)/windows
windows: CFLAGS += -mwindows -I $(EXTERNINCLUDE)/windows -L $(EXTERNLIB)/windows -L $(LIBDIR)
windows: OBJDIR := $(OBJDIR)/windows
windows: BINDIR := $(BINDIR)/windows

init: init_bin init_lib init_obj

init_bin:
ifeq ($(OS), Windows_NT)
	@echo $(shell test -d $(BINDIR) || mkdir -p $(BINDIR))
else
	@echo $(shell mkdir -p $(BINDIR))
endif

init_lib:
ifeq ($(OS), Windows_NT)
	@echo $(shell test -d $(LIBDIR) || mkdir -p $(LIBDIR))
else
	@echo $(shell mkdir -p $(LIBDIR))
endif

init_obj:
ifeq ($(OS), Windows_NT)
	@echo $(shell test -d $(OBJDIR) || mkdir -p $(OBJDIR))
else
	@echo $(shell mkdir -p $(OBJDIR))
endif

UIL     := $(LIBDIR)/linux/libui.a
UIW     := $(LIBDIR)/windows/libui.a
UIOBJSL := $(patsubst $(SRCDIR)/ui/%.c, $(OBJDIR)/linux/ui_%.o, $(wildcard $(SRCDIR)/ui/*.c))
UIOBJSW := $(patsubst $(SRCDIR)/ui/%.c, $(OBJDIR)/windows/ui_%.o, $(wildcard $(SRCDIR)/ui/*.c))

OSL     := $(LIBDIR)/linux/libos.a
OSW     := $(LIBDIR)/windows/libos.a
OSOBJSL := $(patsubst $(SRCDIR)/os/%.c, $(OBJDIR)/linux/os_%.o, $(wildcard $(SRCDIR)/os/*.c))
OSOBJSW := $(patsubst $(SRCDIR)/os/%.c, $(OBJDIR)/windows/os_%.o, $(wildcard $(SRCDIR)/os/*.c))
OSOBJSL += $(patsubst $(SRCDIR)/os/linux/%.c, $(OBJDIR)/linux/os_linux_%.o, $(wildcard $(SRCDIR)/os/linux/*.c))
OSOBJSW += $(patsubst $(SRCDIR)/os/windows/%.c, $(OBJDIR)/windows/os_windows_%.o, $(wildcard $(SRCDIR)/os/windows/*.c))

SERVERL     := $(LIBDIR)/linux/libserver.a
SERVERW     := $(LIBDIR)/windows/libserver.a
SERVEROBJSL := $(patsubst $(SRCDIR)/server/%.c, $(OBJDIR)/linux/server_%.o, $(wildcard $(SRCDIR)/server/*.c))
SERVEROBJSW := $(patsubst $(SRCDIR)/server/%.c, $(OBJDIR)/windows/server_%.o, $(wildcard $(SRCDIR)/server/*.c))

BDL := $(LIBDIR)/linux/libbd.a
BDW := $(LIBDIR)/windows/libbd.a
BDOBJSL := $(patsubst $(SRCDIR)/bd/%.c, $(OBJDIR)/linux/bd_%.o, $(wildcard $(SRCDIR)/bd/*.c))
BDOBJSW := $(patsubst $(SRCDIR)/bd/%.c, $(OBJDIR)/linux/bd_%.o, $(wildcard $(SRCDIR)/bd/*.c))

LOCALL     := $(BINDIR)/linux/local
LOCALW     := $(BINDIR)/windows/local.exe
LOCALOBJSL := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/linux/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))
LOCALOBJSW := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/windows/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))

linux:   init $(LOCALL)
windows: init $(LOCALW)

$(UIL): $(UIOBJSL)
	ar rcs $@ $^

$(UIW): $(UIOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/ui_%.o: $(SRCDIR)/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/ui_%.o: $(SRCDIR)/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OSL): $(OSOBJSL)
	ar rcs $@ $^

$(OSW): $(OSOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/os_%.o: $(SRCDIR)/os/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/os_%.o: $(SRCDIR)/os/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/linux/os_linux_%.o: $(SRCDIR)/os/linux/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/os_windows_%.o: $(SRCDIR)/os/windows/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(SERVERL): $(SERVEROBJSL)
	ar rcs $@ $^

$(SERVERW): $(SERVEROBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/server_%.o: $(SRCDIR)/server/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/server_%.o: $(SRCDIR)/server/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(BDL): $(BDOBJSL)
	ar rcs $@ $^

$(BDW): $(BDOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/bd_%.o: $(SRCDIR)/bd/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/bd_%.o: $(SRCDIR)/bd/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(LOCALL): $(UIL) $(OSL) $(SERVERL) $(BDL) $(LOCALOBJSL)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSL) $(LIBS)

$(LOCALW): $(UIW) $(OSW) $(SERVERW) $(BDW) $(LOCALOBJSW)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSW) $(LIBS)

$(OBJDIR)/linux/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

$(OBJDIR)/windows/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

clean:
	@echo $(shell rm -rf $(BINDIR) $(LIBDIR) $(OBJDIR))

PHONY += clean
.PHONY: $(PHONY)

.SILENT: init init_bin init_obj init_lib
