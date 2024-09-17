/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockDirective.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/10 16:26:34 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	BLOCKDIRECTIVE_HPP
# define BLOCKDIRECTIVE_HPP

# define PR_DIR_TYPE_SIMPLE 1
# define PR_DIR_TYPE_BLOCK 2

# include "Directive.hpp"
# include <deque>
# include <string>

class BlockDirective : public Directive
{
	public:
		typedef std::deque<Directive*>	instructions_t;
	private:
		instructions_t	_instructions;
		bool			_block_opened;
		
	public:
		// BlockDirective(int type, std::deque<std::string> args, std::deque<Directive*> instructions, Directive *context);
		// BlockDirective(int type, std::deque<std::string> args, Directive *context);
		BlockDirective(void);
		BlockDirective(std::deque<std::string> args, std::deque<Directive*> instructions, Directive *context);
		BlockDirective(std::deque<std::string> args, Directive *context);
		~BlockDirective(void);
		const std::deque<Directive*>	&getInstructions(void) const;
		void							addInstruction(Directive *);
		bool							openBlock(void);
		bool							closeBlock(void);
};

#endif