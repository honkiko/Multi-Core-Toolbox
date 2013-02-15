
#GTEST_DIR = ./gtest
GTEST_DIR = /kiko/test/gtest-1.6.0
GTEST_MAIN_A = ./gtest/gtest_main.a

# Flags passed to the preprocessor.
CPPFLAGS += -I.. -I$(GTEST_DIR)

# Flags passed to the C++ compiler.
#CXXFLAGS += -g -MMD -Wall -Wno-sign-compare  
CXXFLAGS += -g -MMD -Wall 

# Flags passed to the linker
LDFLAGS += -lpthread

EXE = ut.x

SRCS = test_spsc_queue.cxx test_mpmc_queue.cxx 

OBJS = $(SRCS:.cxx=.o)
#OBJS = $(subst .cxx,.o,$(SRCS))

DEPSRCS = $(wildcard *.cxx) $(wildcard *.hxx) \
		  $(wildcard ../*.h)

DEPS = $(SRCS:.cxx=.d)

all: $(EXE) 

$(GTEST_MAIN_A):
	make all -C ./gtest

%.o: %.cxx $(DEPSRCS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

$(EXE): $(OBJS) $(GTEST_MAIN_A)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXE) $(OBJS) $(DEPS)

