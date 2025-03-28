# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/06 10:28:10 by jerperez          #+#    #+#              #
#    Updated: 2024/10/17 12:40:43 by nrea             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -g3 -std=c++98	-I./server -I./header_parser -I./config

SRCS_NREA_FILES	=	headerParser.cpp\
					findServer.cpp\
					handle_get.cpp\
					handle_post.cpp\
					handle_delete.cpp\
					handle_get_dir.cpp\
					handle_get_cgi.cpp\
					handle_post_cgi.cpp\
					response_error.cpp\
					response_autoindex.cpp\
					response_redirect.cpp\
					Logger.cpp\
					cgi_utils.cpp\
					utils.cpp\
					FileInfos.cpp

SRCS_NREA	= 	$(addprefix header_parser/, $(SRCS_NREA_FILES))

SRCS_QGIRAUX_FILES	= 	chunk_send.cpp\
						chunk_receive.cpp\
						constructor.cpp\
						generate_error_page.cpp\
						generate_index_page.cpp\
						method_autoindex.cpp\
						method_cgi_get.cpp\
						method_cgi_post.cpp\
						method_delete.cpp\
						method_error.cpp\
						method_get.cpp\
						method_post.cpp\
						method_return.cpp\
						pollin.cpp\
						ServerRun.cpp\
						ServerStart.cpp\
						ServerStop.cpp\
						utils.cpp\



SRCS_QGIRAUX	= 	$(addprefix server/, $(SRCS_QGIRAUX_FILES))

SRCS_JERPEREZ_FILES	= tokenizer.cpp\
			parse.cpp\
			parse_next_directive.cpp\
			DirectiveBlock.cpp\
			Directive.cpp\
			unquote.cpp\
			ConfigServer.cpp\
			ConfigServer_add.cpp\
			ConfigServer_in.cpp\
			ConfigServer_serverCmp.cpp\
			ConfigServer_get.cpp\
			ConfigServer_getD.cpp\
			ConfigServer_getters.cpp\
			ConfigServer_debug.cpp\
			ConfigServer_eval.cpp\
			ConfigBlock_knownDirective.cpp\
			ConfigBlock_in.cpp\
			ConfigBlock_getD.cpp\
			ConfigBlock_addDirective.cpp\
			ConfigBlock_debug.cpp\
			ConfigBlock.cpp\
			ConfigLocation.cpp\

SRCS_JERPEREZ	= 	$(addprefix config/, $(SRCS_JERPEREZ_FILES))


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
	@echo "Settings up forbidden file and dir for defense"
	@chmod 0 html-files/forbidden_directory
	@chmod 0 html-files/forbidden_file.html
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
	@echo "changing back auth for forbidden file and dir"
	@chmod 777 html-files/forbidden_directory
	@chmod 777 html-files/forbidden_file.html
	@echo "$(_COLOR_YELLOW)Program(s) removed.$(_COLOR_END)"

re: fclean all

.PHONY: all re clean fclean
