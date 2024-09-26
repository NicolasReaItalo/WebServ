# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/06 10:28:10 by jerperez          #+#    #+#              #
#    Updated: 2024/09/23 12:37:54 by qgiraux          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -g3 -std=c++98	-I./qgiraux -I./nrea -I./jerperez

SRCS_NREA_FILES	=headerParser.cpp\
				 findServer.cpp\
				 handle_get.cpp\
				 response_error.cpp\
				 response_autoindex.cpp\
				 Logger.cpp\
				 utils.cpp

SRCS_NREA	= 	$(addprefix nrea/, $(SRCS_NREA_FILES))

SRCS_QGIRAUX_FILES	= 	chunk_receive.cpp\
				method_delete.cpp\
				pollin.cpp\
				ServerStart.cpp\
				chunk_send.cpp\
				method_get.cpp\
				ServerStop.cpp\
				method_post.cpp\
				method_error.cpp\
				ServerRun.cpp\
				utils.cpp\
				constructor.cpp\
				generate_error_page.cpp\
				generate_index_page.cpp\
				method_autoindex.cpp\

SRCS_QGIRAUX	= 	$(addprefix qgiraux/, $(SRCS_QGIRAUX_FILES))

SRCS_JERPEREZ_FILES	= tokenizer.cpp\
			parser.cpp\
			DirectiveBlock.cpp\
			Directive.cpp\
			unquote.cpp\
			server_config.cpp\
			ConfigServer.cpp\
			ConfigBlock.cpp\
			ConfigLocation.cpp\

SRCS_JERPEREZ	= 	$(addprefix jerperez/, $(SRCS_JERPEREZ_FILES))


SRCS	=	main.cpp\
			$(SRCS_JERPEREZ)\
			$(SRCS_QGIRAUX)\
			$(SRCS_NREA)

OBJ_DIR	=	obj

OBJ		= 	$(SRCS:.cpp=.o)
OBJS	= 	$(addprefix $(OBJ_DIR)/, $(OBJ))

###############################################

_COLOR_END=\033[0m
_COLOR_BOLD=\033[1m
_COLOR_RED=\033[31m
_COLOR_GREEN=\033[32m
_COLOR_YELLOW=\033[33m

###############################################

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CPPFLAGS) -o $@ $^
	@echo "$(_COLOR_GREEN)Ready to use $(_COLOR_BOLD)$(NAME) !$(_COLOR_END)"

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.o: %.cpp
	@echo Compiling: $<
	@mkdir -p $(@D)
	@$(CXX) $(CPPFLAGS) -MMD -MP -c -o $@ $<

clean:
	@rm -rf $(OBJ_DIR)/
	@echo "$(_COLOR_YELLOW)$(OBJ_DIR) directory removed.$(_COLOR_END)"

fclean: clean
	@rm -f $(NAME) $(NAME_B)
	@echo "$(_COLOR_YELLOW)Program(s) removed.$(_COLOR_END)"

re: fclean all

.PHONY: all re clean fclean
