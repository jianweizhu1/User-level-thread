# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o

CC = gcc
CFLAGS	:= -Wall -Wextra -Werror
CFLAGS	+= -pipe
CFLAGS	+= -MMD

# Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

all: $(lib)

# track object file dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# generate static library from all object files
$(lib): $(objs)
	ar rcs $(lib) $^

# build all object files
%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -rf $(lib) $(objs) $(deps)
