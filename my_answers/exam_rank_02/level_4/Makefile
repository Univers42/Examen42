CFLAGS 	= -Wall -Werror -Wextra -g
INCLUDE = flood_fill.h
EXEC 	= my_program
CC 	= cc
AR 	= ar rcs
NAME 	= libfteval.a
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

all: $(NAME) $(EXEC)

# Rule to compile .c files into .o files
%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the static library
$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Clean object files
clean:
	$(RM) $(OBJS)

# Full clean (remove executable and library)
fclean: clean
	$(RM) $(NAME) $(EXEC)

# Recompile everything
re: fclean all

.PHONY: all clean fclean re
