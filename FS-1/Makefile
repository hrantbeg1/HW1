CC = g++
FLAGS = -Wextra -Wall -Werror
NAME = read-file
SRC = main.cpp
OBJ = main.o

all: $(NAME)

$(NAME): $(OBJ)
	@ $(CC) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ): $(SRC)
	@ $(CC) $(FLAGS) -c $(SRC) -o $(OBJ)

clean:
	@ rm -rf $(NAME) $(OBJ)

fclean:
	@ rm -f $(OBJ)
