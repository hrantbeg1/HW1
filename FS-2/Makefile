NAME = simple-copy
FLAGS = -Wall -Wextra -Werror
GCC = g++
OBJ = main.o
SRC = main.cpp

all: $(NAME)

$(NAME) : $(OBJ)
	@ $(GCC) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ) : $(SRC)
	@ $(GCC) $(FLAGS) -c $(SRC) -o $(OBJ)
       
clean:
	@ rm -rf $(NAME) $(OBJ)

fclean:
	@ rm -f $(OBJ)
