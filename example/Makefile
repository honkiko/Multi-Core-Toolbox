
MCT_DIR = ..

PPFLAGS = -I$(MCT_DIR) 

CCFLAGS = -g 

LDFLAGS = -lpthread

CXXSRC = $(wildcard *.cxx)
CSRC   = $(wildcard *.c)

OBJS += $(CSRC:.c=.o)
OBJS += $(CXXSRC:.cxx=.o)

EXE += $(CSRC:.c=.x) 
EXE += $(CXXSRC:.cxx=.x) 

all: $(OBJS) $(EXE) 


%.o: %.cxx $(MCT_DIR)/spsc_queue.h $(MCT_DIR)/mpmc_queue.h 
	g++ $(PPFLAGS) $(CCFLAGS) -c $<

%.o: %.c $(MCT_DIR)/spsc_queue.h $(MCT_DIR)/mpmc_queue.h 
	gcc $(PPFLAGS) $(CCFLAGS) -c $<

%.x: %.o
	g++ $(CCFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXE) $(OBJS) 

