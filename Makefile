all: release

include configure.mk

CXXFLAGS  = -std=c++11 -pedantic
CXXFLAGS += -Wall -Weffc++ -Wextra -Wcast-align -Wconversion
CXXFLAGS += -Wfloat-equal -Wformat=2 -Wmissing-declarations
CXXFLAGS += -Woverlength-strings -Wshadow -Wunreachable-code
CXXFLAGS += -isystem $(OPENBLAS)/include
LDFLAGS   = -llapack -L$(OPENBLAS)/lib -lopenblas

RELEASE_CXXFLAGS = $(CXXFLAGS) -DNDEBUG -O3 -msse -msse2 -msse3 -mfpmath=sse
RELEASE_LDFLAGS  = $(LDFLAGS)
DEBUG_CXXFLAGS   = $(CXXFLAGS) -g3 -O0
DEBUG_LDFLAGS    = $(LDFLAGS) -rdynamic

include manifest.mk

.PHONY: clean test-release test-debug

clean:
	@ rm -rf bin tmp

install: release
	@ mkdir -p $(INSTALL_PREFIX)/bin
	@ cp -v bin/* $(INSTALL_PREFIX)/bin/

#
# USAGE: $(call .compile, <cpp-file>, <obj-file>, <options>)
#
.compile = \
	printf "+ $(1)\n" && \
	mkdir -p $(shell dirname $(2)) && \
	$(CXX) $(3) -c -o $(2) $(1)

#
# USAGE: $(call .link, <exe-file>, <obj-files>, <linker-options>)
#
.link = \
	printf "+ $(1)\n" && \
	mkdir -p $(shell dirname $(1)) && \
	$(CXX) -o $(1) $(2) $(3)

release: .release
debug: .debug
test-release: .test-release
test-debug: .test-debug
