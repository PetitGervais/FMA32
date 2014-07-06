# To define the base directory
export VPATH += $(CURDIR)/../src:$(CURDIR)/src

ifndef $(VERBOSE)
VERBOSE=false
endif

# The source files: regardless of where they reside in the source tree,
# VPATH will locate them...
GROUP_SRC_CPP = \
    AllocTest.cpp \
    MemoryAllocTest.cpp \
    Blocks.cpp
    
GROUP_SRC_C = \
    memory.c

# Build a Dependency list and an Object list, by replacing the .cpp
# extension to .d for dependency files, and .o for object files.
GROUP_DEP = $(patsubst %.cpp, deps-$(CFG)/%.cpp.d, ${GROUP_SRC_CPP})
GROUP_DEP += $(patsubst %.c, deps-$(CFG)/%.c.d, ${GROUP_SRC_C})
GROUP_OBJ = $(patsubst %.cpp, objs-$(CFG)/%.cpp.o, ${GROUP_SRC_CPP})
GROUP_OBJ += $(patsubst %.c, objs-$(CFG)/%.c.o, ${GROUP_SRC_C})

# Your final binary
TARGET=AllocTest

# define compiler
CXX = g++
GCC = gcc

# What compiler to use for generating dependencies: 
# it will be invoked with -MM -MP
CXXDEP = $(CXX) -std=c++0x
CDEP = $(GCC)

# Separate compile options per configuration
ifeq ($(CFG),debug)
INCLUDEFLAGS += -I$(CURDIR)/src -I$(CURDIR)/../src
CXXFLAGS += -g -pg -Wall -std=c++0x -DTEST_MODE ${INCLUDEFLAGS}
CFLAGS += -g -pg -Wall -DTEST_MODE ${INCLUDEFLAGS}
else
INCLUDEFLAGS += -I$(CURDIR)/src -I$(CURDIR)/../src
CXXFLAGS += -Wall -std=c++0x ${INCLUDEFLAGS}
CFLAGS += -Wall -DTEST_MODE ${INCLUDEFLAGS}
endif

# A common link flag for all configurations
LDFLAGS += -pg

all:	inform bin-$(CFG)/${TARGET}

inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	@echo "Invalid configuration "$(CFG)" specified."
	@echo "You have to specify a configuration when running make : 'make CFG=debug' or 'make CFG=release'" 
	@echo  "You can also have a verbose mode with VERBOSE=true"
	@exit 1
endif
endif
	@echo "Configuration "$(CFG)
	@echo "------------------------"

bin-$(CFG)/${TARGET}: ${GROUP_OBJ}
ifeq ($(VERBOSE),false)
	@mkdir -p $(dir $@)
	@echo "Link => " ${TARGET}
	@$(CXX) -g ${LDFLAGS} -o $@ $^
else
	@mkdir -p $(dir $@)
	$(CXX) -g ${LDFLAGS} -o $@ $^
endif

objs-$(CFG)/%.cpp.o: %.cpp
ifeq ($(VERBOSE),false)
	@mkdir -p $(dir $@)
	@echo "Compile => " $<
	@$(CXX) -c $(CXXFLAGS) -o $@ $<
else
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
endif

objs-$(CFG)/%.c.o: %.c
ifeq ($(VERBOSE),false)
	@mkdir -p $(dir $@)
	@echo "Compile => " $<
	@$(GCC) -c $(CFLAGS) -o $@ $<
else
	@mkdir -p $(dir $@)
	$(GCC) -c $(CFLAGS) -o $@ $<
endif

deps-$(CFG)/%.cpp.d: %.cpp
	@mkdir -p $(dir $@)
	@echo "Generating dependencies for " $<
	@set -e ; $(CXXDEP) -MM -MP $(INCLUDEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,objs-$(CFG)\/\1.cpp.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
deps-$(CFG)/%.c.d: %.c
	@mkdir -p $(dir $@)
	@echo "Generating dependencies for " $<
	@set -e ; $(CDEP) -MM -MP $(INCLUDEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,objs-$(CFG)\/\1.c.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	@rm -rf \
	deps-debug objs-debug bin-debug \
	deps-release objs-release bin-release

# Unless "make clean" is called, include the dependency files
# which are auto-generated. Don't fail if they are missing
# (-include), since they will be missing in the first invocation!
ifneq ($(MAKECMDGOALS),clean)
-include ${GROUP_DEP}
endif

