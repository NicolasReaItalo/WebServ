/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectiveBlock.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:35:20 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	DIRECTIVEBLOCK_HPP
# define DIRECTIVEBLOCK_HPP

# define PR_DIR_TYPE_SIMPLE 1
# define PR_DIR_TYPE_BLOCK 2

# include "Directive.hpp"
# include <deque>
# include <string>

class DirectiveBlock : public Directive
{
	public:
		typedef std::deque<DirectiveBlock> \
								instructions_t;
	private:
		int						_block_state;
		instructions_t			_instructions;
		
	public:
		DirectiveBlock(void);
		~DirectiveBlock(void);
		const instructions_t	&getInstructions(void) const;
		//
		void					addInstruction(const DirectiveBlock &d);
		DirectiveBlock			&nextInstructionRef(void);
		//
		int						openBlock(void);
		int						closeBlock(void);
};

#endif