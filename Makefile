CC = cc
SRC = $(wildcard src/*.c)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
CFLAGS = -Iinclude -I./MLX42/include/MLX42
LDFLAGS = ./MLX42/build/libmlx42.a

UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	LDFLAGS += -lglfw -ldl -pthread -lm
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -lglfw -L $(shell brew --prefix glfw)/lib -framework Cocoa -framework IOKit
endif

NAME = ips

all: mlx $(NAME)

mlx:
	@cmake -B ./MLX42/build ./MLX42
	@cmake --build ./MLX42/build -j16

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

build/%.o: src/%.c $(HR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf build && rm -rf MLX42/build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
