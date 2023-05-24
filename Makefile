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
P99                := false

CFLAGS := \
	-std=$(STDC) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fno-strict-aliasing -ffast-math -fmax-errors=1 \
	-D_GNU_SOURCE -I $(EXTERNINCLUDE) -static --static -static-libgcc -include $(SRCINCLUDE)/trivia.h

CXXFLAGS := \
	-std=$(STDCXX) -O$(OPTIMIZATION_LEVEL) -DNCURSES_STATIC -fno-strict-aliasing -ffast-math -fmax-errors=1 \
	-fpermissive -D_GNU_SOURCE -I $(EXTERNINCLUDE) -static --static -static-libgcc -static-libstdc++ \
	-include $(SRCINCLUDE)/trivia.h

ifeq ($(RUNTIME_DIAGS), false)
	CFLAGS += -DDISABLE_RUNTIME_DIAGS
	CXXFLAGS += -DISABLE_RUNTIME_DIAGS
endif

ifeq ($(P99), true)
	CFLAGS += -DTRIVIA_USE_P99
	CXXFLAGS += -DTRIVIA_USE_P99
endif

DFLAGS := \
	-ggdb3 -pg -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow=local -Wstrict-prototypes -Wwrite-strings -Wconversion \
	-Wcast-align -Wnull-dereference -Wformat=2 -Wno-format-y2k -Wnonnull

DXXFLAGS := \
	-ggdb3 -pg -Wall -Wextra -Winline -Wpointer-arith -Wfloat-equal -Wundef \
	-Wshadow=local -Wwrite-strings -Wconversion -Wnonnull \
	-Wcast-align -Wnull-dereference -Wformat=2 -Wno-format-y2k

UILIBS     := -lformw -lmenuw -lpanelw -lncursesw
OSLIBS     := -lbacktrace
DBLIBS     := -lsqlite3 -lm
JSONLIBS   := -lcjson -lcjson_utils
TRIVIALIBS := -lui -los -lserver -ldb -ladt -lconfig
LIBS       =  $(TRIVIALIBS) $(UILIBS) $(OSLIBS) $(DBLIBS) $(JSONLIBS)

ifeq ($(OS), Windows_NT)
all: windows
else
all: linux windows
endif

linux: CC        := $(GCC_LINUX)
linux: CXX       := $(GXX_LINUX)
linux: LIBDIR    := $(LIBDIR)/linux
linux: CFLAGS    += -I $(EXTERNINCLUDE)/linux -L $(EXTERNLIB)/linux -L $(LIBDIR) -pthread -lpthread
linux: CXXFLAGS  += -I $(EXTERNINCLUDE)/linux -L $(EXTERNLIB)/linux -L $(LIBDIR) -pthread -lpthread
linux: OBJDIR    := $(OBJDIR)/linux
linux: BINDIR    := $(BINDIR)/linux
linux: BINRESDIR := $(BINDIR)/resources

windows: CC        := $(GCC_WINDOWS)
windows: CXX       := $(GXX_WINDOWS)
windows: LIBDIR    := $(LIBDIR)/windows
windows: CFLAGS    += -mwindows -municode -mthreads -I $(EXTERNINCLUDE)/windows -L $(EXTERNLIB)/windows -L $(LIBDIR)
windows: CXXFLAGS  += -mwindows -municode -mthreads -I $(EXTERNINCLUDE)/windows -L $(EXTERNLIB)/windows -L $(LIBDIR)
windows: OBJDIR    := $(OBJDIR)/windows
windows: BINDIR    := $(BINDIR)/windows
windows: BINRESDIR := $(BINDIR)/resources

init: init_bin init_lib init_obj

init_bin:
ifeq ($(OS), Windows_NT)
	@echo $(shell test -d $(BINDIR) || mkdir -p $(BINRESDIR))
	@echo $(shell Copy-Item $(RESDIR)/questions.json,$(RESDIR)/db.sqlite -Destination $(BINDIR)/resources/ -Passthru)
else
	@echo $(shell mkdir -p $(BINRESDIR))
	@echo $(shell cp $(RESDIR)/questions.json $(RESDIR)/db.sqlite $(BINRESDIR))
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

CONFIGL     := $(LIBDIR)/linux/libconfig.a
CONFIGW     := $(LIBDIR)/windows/libconfig.a
CONFIGOBJSL := $(patsubst $(SRCDIR)/config/%.c, $(OBJDIR)/linux/config_%.o, $(wildcard $(SRCDIR)/config/*.c))
CONFIGOBJSW := $(patsubst $(SRCDIR)/config/%.c, $(OBJDIR)/windows/config_%.o, $(wildcard $(SRCDIR)/config/*.c))

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

DBL     := $(LIBDIR)/linux/libdb.a
DBW     := $(LIBDIR)/windows/libdb.a
DBOBJSL := $(patsubst $(SRCDIR)/local/db/%.c, $(OBJDIR)/linux/db_%.o, $(wildcard $(SRCDIR)/local/db/*.c))
DBOBJSW := $(patsubst $(SRCDIR)/local/db/%.c, $(OBJDIR)/windows/db_%.o, $(wildcard $(SRCDIR)/local/db/*.c))

LOCALL     := $(BINDIR)/linux/local
LOCALW     := $(BINDIR)/windows/local.exe
LOCALOBJSL := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/linux/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))
LOCALOBJSW := $(patsubst $(SRCDIR)/local/ui/%.c, $(OBJDIR)/windows/local_ui_%.o, $(wildcard $(SRCDIR)/local/ui/*.c))

REMOTEL     := $(BINDIR)/linux/remote
REMOTEW     := $(BINDIR)/windows/remote.exe
REMOTEOBJSL := $(patsubst $(SRCDIR)/remote/ui/%.cpp, $(OBJDIR)/linux/remote_ui_%.o, $(wildcard $(SRCDIR)/remote/ui/*.cpp))
REMOTEOBJSL += $(patsubst $(SRCDIR)/remote/questionhandler/%.cpp, $(OBJDIR)/linux/remote_questionhandler_%.o, $(wildcard $(SRCDIR)/remote/questionhandler/*.cpp))
REMOTEOBJSW := $(patsubst $(SRCDIR)/remote/ui/%.cpp, $(OBJDIR)/windows/remote_ui_%.o, $(wildcard $(SRCDIR)/remote/ui/*.cpp))
REMOTEOBJSW += $(patsubst $(SRCDIR)/remote/questionhandler/%.cpp, $(OBJDIR)/windows/remote_questionhandler_%.o, $(wildcard $(SRCDIR)/remote/questionhandler/*.cpp))

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

$(CONFIGL): $(CONFIGOBJSL)
	ar rcs $@ $^

$(CONFIGW): $(CONFIGOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/config_%.o: $(SRCDIR)/config/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/config_%.o: $(SRCDIR)/config/%.c
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

$(DBL): $(DBOBJSL)
	ar rcs $@ $^

$(DBW): $(DBOBJSW)
	ar rcs $@ $^

$(OBJDIR)/linux/db_%.o: $(SRCDIR)/local/db/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)/windows/db_%.o: $(SRCDIR)/local/db/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(LOCALL): $(ADTL) $(CONFIGL) $(UIL) $(OSL) $(SERVERL) $(DBL) $(LOCALOBJSL)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSL) $(LIBS)

$(LOCALW): $(ADTW) $(CONFIGW) $(UIW) $(OSW) $(SERVERW) $(DBW) $(LOCALOBJSW)
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -o $@ $(SRCDIR)/local/main.c $(LOCALOBJSW) $(RESDIR)/icon.o $(LIBS) -lws2_32

$(OBJDIR)/linux/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

$(OBJDIR)/windows/local_ui_%.o: $(SRCDIR)/local/ui/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -include $(SRCINCLUDE)/local.h -c $< -o $@

$(REMOTEL): $(ADTL) $(CONFIGL) $(UIL) $(OSL) $(SERVERL) $(DBL) $(REMOTEOBJSL)
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -o $@ $(SRCDIR)/remote/main.cpp $(REMOTEOBJSL) $(LIBS)

$(REMOTEW): $(ADTW) $(CONFIGW) $(UIW) $(OSW) $(SERVERW) $(DBW) $(REMOTEOBJSW)
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -o $@ $(SRCDIR)/remote/main.cpp $(REMOTEOBJSW) $(RESDIR)/icon.o $(LIBS) -lws2_32

$(OBJDIR)/linux/remote_ui_%.o: $(SRCDIR)/remote/ui/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

$(OBJDIR)/windows/remote_ui_%.o: $(SRCDIR)/remote/ui/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

$(OBJDIR)/linux/remote_questionhandler_%.o: $(SRCDIR)/remote/questionhandler/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

$(OBJDIR)/windows/remote_questionhandler_%.o: $(SRCDIR)/remote/questionhandler/%.cpp
	$(CXX) $(CXXFLAGS) $(DXXFLAGS) -include $(SRCINCLUDE)/remote.h -c $< -o $@

clean:
	@echo $(shell rm -rf $(BINDIR) $(LIBDIR) $(OBJDIR))

PHONY += clean
.PHONY: $(PHONY)

.SILENT: init init_bin init_obj init_lib
