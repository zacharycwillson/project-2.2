# Target library
lib := libuthread.a

all: $(lib)

# Sources, objects, and dependency files
srcs := queue.c uthread.c context.c preempt.c sem.c
objs := $(srcs:.c=.o)
deps := $(objs:.o=.d)

# Toolchain
CC       := gcc
AR       := ar
ARFLAGS  := rcs

# Compiler flags
CFLAGS   := -Wall -Wextra -Werror -MMD -MP -D_XOPEN_SOURCE=600

# Archive rule
$(lib): $(objs)
	@echo "AR   $@"
	$(AR) $(ARFLAGS) $@ $^

# Pattern rule
%.o: %.c
	@echo "CC   $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Pull in dependency files 
-include $(deps)

# Clean up all generated files
.PHONY: clean
clean:
	@echo "CLEAN"
	rm -f $(lib) $(objs) $(deps)
