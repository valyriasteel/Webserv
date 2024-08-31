NAME = webserv
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
CXX = c++ -g
SRCS = main.cpp \
		Server.cpp \
		Location.cpp
OBJS_DIR = obj/
OBJS = main.o \
		Server.o \
		Location.o
OBJS_PRE = $(OBJS_DIR)main.o \
			$(OBJS_DIR)Server.o \
			$(OBJS_DIR)Location.o

RM = rm -rf

RED = \x1b[31;01m
GREEN = \x1b[32;01m
BLUE = \033[34;1m
RESET = \x1b[0m

all: $(NAME)

$(NAME): $(OBJS_PRE)
	@$(CXX) $(CXXFLAGS) $(OBJS_PRE) -o $(NAME)
	@printf "$(GREEN) Executable named Webserv created$(RESET)\n"

$(OBJS_DIR)%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJS_DIR)
	@printf "$(BLUE) Deleting object files$(RESET)\n"

fclean:
	@$(RM) $(NAME) $(OBJS_DIR)
	@printf "$(RED) Deleting executable filename and object file$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re