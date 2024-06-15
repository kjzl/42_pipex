NAME = pipex
CFLAGS = -Wall -Wextra -Werror -O3 -flto
# CFLAGS = -g -Og -fanalyzer

LIBFTDIR = ./libft
LIBFT = $(LIBFTDIR)/libft.a

SRCS = \
	cmd.c\
	cmd2.c\
	pipex.c\
	cmd_str_to_argv.c\
	util.c\
	bonus.c\

OBJ = $(SRCS:.c=.o)

%.o: %.c
	cc $(CFLAGS) -c -I$(LIBFTDIR) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C $(LIBFTDIR) CFLAGS='$(CFLAGS)'
	cc $(CFLAGS) $(OBJ) -L$(LIBFTDIR) -lft -o $(NAME)

bonus: CFLAGS += -DBONUS
bonus: all

clean:
	$(MAKE) -C $(LIBFTDIR) clean
	rm -f $(OBJ)

fclean: clean
	$(MAKE) -C $(LIBFTDIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
