CC=gcc
CFLAGS=-o
TARGETS=ordinary_pipe line_a wait_child two_pipe line_a_2 num_of_process
all: $(TARGETS)

.PHONY: all
%:
	$(CC) -o $@ $@.c 
       
clean:
	rm $(TARGETS)
