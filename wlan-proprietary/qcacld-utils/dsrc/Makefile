# run tests on the host platform, everything else is cross-compiled
# Note: if running both testing and cross-compiling for target, make
# sure and run make clean between targets.  The object files
# for testing and for cross-compiling are the same but compiled
# for different platforms so they need to be deleted
ifdef CROSS_COMPILE
    CC   := $(CROSS_COMPILE)gcc
    CXX  := $(CROSS_COMPILE)g++
endif

ifeq ($(MAKECMDGOALS),test)
    CFLAGS+=-g
endif

CFLAGS += -Wall -Werror
CPPFLAGS += -MMD -MP -I inc

# Distinguish between libnl-2.0 and libnl-3.0
ifdef HAVE_LIBNL3
    CPPFLAGS += -I /usr/arm-linux-gnueabihf/include/libnl3
    LDFLAGS := -lnl-3 -lnl-genl-3
else
    ifdef HAVE_LIBNL
        LDFLAGS := -lnl -lnl-genl
    endif
endif

OBJDIR = $(CROSS_COMPILE)obj
BINDIR = $(CROSS_COMPILE)bin

VPATH=src
CPPFLAGS += -I src

#-------------------------------------------------------------------------------
#  rules specific to this project
#-------------------------------------------------------------------------------

all: $(BINDIR)/dsrc_rx $(BINDIR)/dsrc_tx $(BINDIR)/dsrc_set_chan

docs:
	$(MAKE) -C doxy

test: unit_test
	./unit_test

TEST_SRCS: unit_test.c dsrc_util.c dsrc_sock.c strlcpy.c
RX_SRCS: dsrc_rx.c dsrc_util.c crc.c dsrc_sock.c strlcpy.c
TX_SRCS: dsrc_tx.c dsrc_util.c crc.c strlcpy.c
ALL_SRCS: $(TEST_SRCS) $(RX_SRCS) $(TX_SRCS)

unit_test: unit_test.o dsrc_util.o strlcpy.o
$(BINDIR)/dsrc_rx: $(OBJDIR)/dsrc_rx.o $(OBJDIR)/dsrc_util.o $(OBJDIR)/crc.o $(OBJDIR)/dsrc_sock.o $(OBJDIR)/strlcpy.o
$(BINDIR)/dsrc_tx: $(OBJDIR)/dsrc_tx.o $(OBJDIR)/dsrc_util.o $(OBJDIR)/crc.o $(OBJDIR)/dsrc_sock.o $(OBJDIR)/strlcpy.o
$(BINDIR)/dsrc_set_chan: $(OBJDIR)/dsrc_set_chan.o $(OBJDIR)/dsrc_util.o $(OBJDIR)/strlcpy.o

help:
	@echo "------------------------------------------------------------------------------"
	@echo "By default this build will attempt to cross compile for \"arm-linux-gnueabihf-\""
	@echo "variant."
	@echo ""
	@echo "To cross compile for a specific a target:"
	@echo "CROSS_COMPILE=arm-oe-linux-gnueabi- make all"
	@echo ""
	@echo "To cross compile using native gcc:"
	@echo "CROSS_COMPILE= make all"
	@echo ""
	@echo "Make targets are:"
	@echo ""
	@echo "all               - all components of this project, except documentation."
	@echo "clean             - clean all components, except documentation."
	@echo "docs              - build documentation. see docs/html/index.html. Requires doxygen 1.8.10"
	@echo "docs_clean        - clean the documentation."
	@echo "------------------------------------------------------------------------------"

#-------------------------------------------------------------------------------
#  nl80211 rules
#-------------------------------------------------------------------------------

dsrc_config: $(BINDIR) $(BINDIR)/dsrc_config

$(BINDIR)/dsrc_config: $(OBJDIR)/dsrc_config.o $(OBJDIR)/dsrc_nl.o $(OBJDIR)/dsrc_dcc.o
	${CC} $(OBJDIR)/dsrc_config.o $(OBJDIR)/dsrc_dcc.o $(OBJDIR)/dsrc_nl.o \
		-o $(BINDIR)/dsrc_config ${LDFLAGS}
	cp dcc.dat $(BINDIR)/dcc.dat

#-------------------------------------------------------------------------------
#  generic rules
#-------------------------------------------------------------------------------

$(BINDIR) $(OBJDIR):
	@mkdir -p $@

$(BINDIR)/%: | $(BINDIR)
	$(CC) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR) $(OBJDIR)

docs_clean:
	rm -rf docs

.PHONY:  test all clean docs docs_clean dsrc_config help

-include $(ALL_SRCS:%.c=$(OBJDIR)/%.d)
