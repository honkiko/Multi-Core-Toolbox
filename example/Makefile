
GTEST_DIR = ../gtest-1.6.0
GTEST_MAIN_A = ../gtest-1.6.0/make/gtest_main.a

MCT_DIR = ..

# Flags passed to the preprocessor.
PPFLAGS += -I$(MCT_DIR) -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
#CCFLAGS += -g -MMD -Wall -Wno-sign-compare  
CCFLAGS += -g -MMD 

# Flags passed to the linker
LDFLAGS += -lpthread

EXE = example_spsc_queue

SRCS = example_spsc_queue.c

OBJS = $(SRCS:.c=.o)
#OBJS = $(subst .c,.o,$(SRCS))

DEPS = $(SRCS:.c=.d)

all: $(EXE) 

%.o: %.c $(MCT_DIR)/spsc_queue.h 
	$(CXX) $(PPFLAGS) $(CCFLAGS) -c $<

$(EXE): $(OBJS) 
	$(CXX) $(CCFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXE) $(OBJS) $(DEPS)

