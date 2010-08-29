CFLAGS += -DCONNECT_URL="\"http://demon9.net/connect/\""
CFLAGS += -DLISTEN_PORT=6667
CFLAGS += -DTARGET_PORT=6697
STRIP_BINS=y

# These should be good for most systems, adjust if not...
DISTCC := /usr/bin/distcc
CCACHE := /usr/bin/ccache

all: world

# Is DISTCC_HOSTS set?
ifneq (y${DISTCC_HOSTS}, y)
# Try to use distcc, if available
ifeq (${DISTCC}, $(wildcard ${DISTCC}))
CC := ${DISTCC}
endif
endif

# Try to use ccache, if available
ifeq (${CCACHE}, $(wildcard ${CCACHE}))
CC := ${CCACHE} ${CC}
endif
 
CFLAGS += -Os -g -pipe
CFLAGS += -I.
CFLAGS += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC
warn_noerror := -Wall -Wno-unused -Wno-strict-aliasing -ansi -std=c99
warn_flags := ${warn_noerror} #-Werror
LDFLAGS := -lm

bin := ircd-reject-nonssl

objs += .obj/ev.o
objs += .obj/evt.o
objs += .obj/main.o
objs += .obj/signal_handler.o
objs += .obj/str.o
objs += .obj/support.o

ifeq (${DEBUG}, y)
STRIP_BINS=n
CFLAGS += -DDEBUG
endif

extra_clean += ${bin}.log

world: ${bin}
${bin}: ${objs}
	@${CC} -o $@ ${LDFLAGS} ${extra_libs} $^
ifeq (${STRIP_BINS}, y)
	strip ${bin}
endif

clean:
	${RM} ${objs} ${bin} ${extra_clean}

distclean:
	@${MAKE} clean

# everything else is good...
.obj/%.o:%.c
	@echo "[CC] $< => $@"
	@${CC} ${warn_flags} ${CFLAGS} -o $@ -c $<

export DISTCC_HOSTS
