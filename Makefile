SRCDIR        := src
SRCINCLUDE    := $(SRCDIR)/include
OBJDIR        := obj
LIBDIR        := lib
EXTERNDIR     := extern
EXTERNINCLUDE := $(EXTERNDIR)/include
EXTERNLIB     := $(EXTERNDIR)/lib
BINDIR        := bin
RESDIR        := resources

ifeq ($(OS), Windows_NT)
	GCC_LINUX   := desdewindowsnopuedes
	GXX_LINUX   := desdewindowsnopuedes

	GCC_WINDOWS := gcc
	GXX_WINDOWS := g++
else
	GCC_LINUX   := $(shell which gcc)
	GXX_LINUX   := $(shell which g++)

	GCC_WINDOWS := $(shell which x86_64-w64-mingw32-gcc)
	GXX_WINDOWS := $(shell which x86_64-w64-mingw32-g++)
endif

STDC               := gnu11
STDCXX             := gnu++11
OPTIMIZATION_LEVEL := 3
RUNTIME_DIAGS      := true

CFLAGS := \
	-std=$(STDC) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fno-strict-aliasing -ffast-math -fmax-errors=1 \
	-D_GNU_SOURCE -I $(EXTERNINCLUDE) -static --static -static-libgcc -include $(SRCINCLUDE)/trivia.h

CXXFLAGS := \
	-std=$(STDCXX) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fno-strict-aliasing -ffast-math -fmax-errors=1 \
	-fpermissive -D_GNU_SOURCE -I $(EXTERNINCLUDE) -static --static -static-libgcc -static-libstdc++ \
	-include $(SRCINCLUDE)/trivia.h

ifeq ($(RUNTIME_DIAGS), false)
	CFLAGS += -DDISABLE_RUNTIME_DIAGS
	CXXFLAGS += --DISABLE_RUNTIME_DIAGS
endif

DFLAGS := \
	-ggdb3 -pg -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow=local -Wstrict-prototypes -Wwrite-strings -Wconversion \
	-Wcast-align -Wnull-dereference -Wformat=2 -Wno-format-y2k -Wnonnull

DXXFLAGS := \
	-ggdb3 -pg -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow=local -Wwrite-strings -Wconversion -Wnonnull \
	-Wcast-align -Wnull-dereference -Wformat=2 -Wno-format-y2k

UILIBS     := -lformw -lmenuw -lpanelw -lncursesw -lm
OSLIBS     := -lbacktrace
BDLIBS     := -lsqlite3
JSONLIBS   := -lcjson -lcjson_utils
TRIVIALIBS := -lui -los -lserver -lbd -ladt
LIBS       =  $(TRIVIALIBS) $(UILIBS) $(OSLIBS) $(BDLIBS) $(JSONLIBS)

ifeq ($(OS), Windows_NT)
all: windows
else
all: linux windows
endif

linux: CC       := $(GCC_LINUX)
linux: CXX      := $(GXX_LINUX)
linux: LIBDIR   := $(LIBDIR)/linux
linux: CFLAGS   += -I $(EXTERNINCLUDE)/linux -L $(EXTERNLIB)/linux -L $(LIBDIR) -pthread -lpthread
linux: CXXFLAGS += -I $(EXTERNINCLUDE)/linux -L $(EXTERNLIB)/linux -L $(LIBDIR) -pthread -lpthread
linux: OBJDIR   := $(OBJDIR)/linux
linux: BINDIR   := $(BINDIR)/linux

windows: CC       := $(GCC_WINDOWS)
windows: CXX      := $(GXX_WINDOWS)
windows: LIBDIR   := $(LIBDIR)/windows
windows: CFLAGS   += -mwindows -municode -mthreads -I $(EXTERNINCLUDE)/windows -L $(EXTERNLIB)/windows -L $(LIBDIR) -lws2_32
windows: CXXFLAGS += -mwindows -municode -mthreads -I $(EXTERNINCLUDE)/windows -L $(EXTERNLIB)/windows -L $(LIBDIR) -lws2_32
windows: OBJDIR   := $(OBJDIR)/windows
windows: BINDIR   := $(BINDIR)/windows

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

ADTL     := $(LIBDIR)/linux/libadt.a
ADTW     := $(LIBDIR)/windows/libadt.a
ADTOBJSL := $(patsubst $(SRCDIR)/adt/%.c, $(OBJDIR)/linux/adt_%.o, $(wildcard $(SRCDIR)/adt/*.c))
ADTOBJSW := $(patsubst $(SRCDIR)/adt/%.c, $(OBJDIR)/windows/adt_%.o, $(wildcard $(SRCDIR)/adt/*.c))

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
BDOBJSL := $(patsubst $(SRCDIR)/local/bd/%.c, $(OBJDIR)/linux/bd_%.o, $(wildcard $(SRCDIR)/local/bd/*.c))
BDOBJSW := $(patsubst $(SRCDIR)/local/bd/%.c, $(OBJDIR)/windows/bd_%.o, $(wildcard $(SRCDIR)/local/bd/*.c))

LOCALL     := $(BINDIR)/linux/local
LOCALW     := $(BINDIR)/windows/local.exe
LOCALOBJSL := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/linux/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))
LOCALOBJSW := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/windows/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))

REMOTEL     := $(BINDIR)/linux/remote
REMOTEW     := $(BINDIR)/windows/remote.exe
REMOTEOBJSL := $(patsubst $(SRCDIR)/remote/ui/%.cpp, $(OBJDIR)/linux/remote_ui_%.o, $(wildcard $(SRCDIR)/remote/ui/*.cpp))
REMOTEOBJSW := $(patsubst $(SRCDIR)/remote/ui/%.cpp, $(OBJDIR)/windows/remote_ui_%.o, $(wildcard $(SRCDIR)/remote/ui/*.cpp))

linux:   init $(LOCALL) $(REMOTEL)
windows: init $(LOCALW) $(REMOTEW)

$(ADTL): $(ADTOBJSL)
	ar rcs $@ $^

$(ADTW): $(ADTOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/adt_%.o: $(SRCDIR)/adt/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/adt_%.o: $(SRCDIR)/adt/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

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

$(OBJDIR)/linux/bd_%.o: $(SRCDIR)/local/bd/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/bd_%.o: $(SRCDIR)/local/bd/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(LOCALL): $(ADTL) $(UIL) $(OSL) $(SERVERL) $(BDL) $(LOCALOBJSL)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSL) $(LIBS)

$(LOCALW): $(ADTW) $(UIW) $(OSW) $(SERVERW) $(BDW) $(LOCALOBJSW)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSW) $(RESDIR)/icon.o $(LIBS)

$(OBJDIR)/linux/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

$(OBJDIR)/windows/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

$(REMOTEL): $(ADTL) $(UIL) $(OSL) $(SERVERL) $(BDL) $(REMOTEOBJSL)
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -o $@ $(SRCDIR)/remote/main.cpp $(REMOTEOBJSL) $(LIBS)

$(REMOTEW): $(ADTW) $(UIW) $(OSW) $(SERVERW) $(BDW) $(REMOTEOBJSW)
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -o $@ $(SRCDIR)/remote/main.cpp $(REMOTEOBJSW) $(RESDIR)/icon.o $(LIBS)

$(OBJDIR)/linux/remote_ui_%.o: $(SRCDIR)/remote/ui/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

$(OBJDIR)/windows/remote_ui_%.o: $(SRCDIR)/remote/ui/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

clean:
	@echo $(shell rm -rf $(BINDIR) $(LIBDIR) $(OBJDIR))

PHONY += clean
.PHONY: $(PHONY)

.SILENT: init init_bin init_obj init_lib
