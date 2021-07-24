NAME		=	Matt_daemon

UNAME		:= $(shell uname)

CC		=	clang++

FLAGS		=	-g -std=c++17 -pthread

EXTRA_FLAGS	=	-lstdc++fs

DELTA		=	$$(echo "$$(tput cols)-47"|bc)

SRC_DIR		=	Sources/
INC_DIR		=	Includes/
OBJ_DIR		=	.objs/

SRC_BASE	=   main.cpp				\
				Daemon.cpp				\
				Check_user_input.cpp	\
				Tintin_reporter.cpp		\


SRCS		=	$(addprefix $(SRC_DIR), $(SRC_BASE))
OBJS		=	$(addprefix $(OBJ_DIR), $(SRC_BASE:.cpp=.o))
NB			=	$(words $(SRC_BASE))
INDEX		=	0

all:
	@make --no-print-directory -j $(NAME)

$(NAME):	$(OBJ_DIR) $(OBJS)
	@$(CC) $(OBJS) -o $(NAME)	\
	-I $(INC_DIR)				\
	$(LIBS)                 	\
	$(FLAGS) $(EXTRA_FLAGS)
	@strip -x $@
	@printf "\r\033[48;5;15;38;5;25m✅   MAKE $(NAME)\033[0m\033[K\n"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(dir $(OBJS))

$(OBJ_DIR)%.o :	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@$(eval DONE=$(shell echo $$(($(INDEX)*20/$(NB)))))
	@$(eval PERCENT=$(shell echo $$(($(INDEX)*100/$(NB)))))
	@$(eval COLOR=$(shell echo $$(($(PERCENT)%35+196))))
	@$(eval TO_DO=$(shell echo $$((20-$(INDEX)*20/$(NB)))))
	@printf "\r\033[38;5;11m⌛ MAKE %10.10s : %2d%% \033[48;5;%dm%*s\033[0m%*s\033[48;5;255m \033[0m\033[38;5;11m %*.*s\033[0m\033[K" $(NAME) $(PERCENT) $(COLOR) $(DONE) "" $(TO_DO) "" $(DELTA) $(DELTA) "$@"
	@$(CC) -MMD -c $< -o $@ -I $(INC_DIR) $(FLAGS)
	@$(eval INDEX=$(shell echo $$(($(INDEX)+1))))

clean:
	@rm -rf $(OBJ_DIR)
	@printf "\r\033[38;5;202m✖  clean $(NAME)\033[0m\033[K\n"

fclean:	clean
	@rm -f $(NAME)
	@printf "\r\033[38;5;196m❌ fclean $(NAME)\033[0m\033[K\n"

re:		fclean all

-include $(OBJS:.o=.d)
