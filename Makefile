CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = ft_ping
SRC = src/ft_ping.c
OBJ = $(SRC:.c=.o)
INC = -Iinc/

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re