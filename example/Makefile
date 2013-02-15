
MCT_DIR = ..

PPFLAGS = -I$(MCT_DIR) 

CCFLAGS = -g -MMD 

LDFLAGS += -lpthread

EXE = example_spsc_queue.x example_mpmc_queue.x 

SRCS = example_spsc_queue.c example_mpmc_queue.c

OBJS = $(SRCS:.c=.o)
#OBJS = $(subst .c,.o,$(SRCS))

DEPS = $(SRCS:.c=.d)

all: $(EXE) 

%.o: %.c $(MCT_DIR)/spsc_queue.h $(MCT_DIR)/mpmc_queue.h 
	gcc $(PPFLAGS) $(CCFLAGS) -c $<

%.x: %.o
	gcc $(CCFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXE) $(OBJS) $(DEPS)

