
MCT_DIR = ..

PPFLAGS = -I$(MCT_DIR) 

CCFLAGS = -g 

LDFLAGS = -lpthread

EXE = example_spsc_queue.x \
	  example_mpmc_queue.x \
	  example_stl_queue.x

SRCS = example_spsc_queue.c \
	   example_mpmc_queue.c \
	   example_stl_queue.cxx

OBJS = $(SRCS:.c=.o)

all: $(EXE) 

example_stl_queue.x: example_stl_queue.cc
	g++ -g -lpthread $< -o $@

%.o: %.c $(MCT_DIR)/spsc_queue.h $(MCT_DIR)/mpmc_queue.h 
	gcc $(PPFLAGS) $(CCFLAGS) -c $<

%.x: %.o
	gcc $(CCFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXE) $(OBJS) 

