/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 10:52:33 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/20 12:21:19 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "ConfigBlock.hpp"

class ConfigLocation : public ConfigBlock
{
	private:
		std::string	_uri;
	public:
		ConfigLocation(void);
		ConfigLocation(const std::string &uri);
		~ConfigLocation(void);
		ConfigLocation(const ConfigLocation &other);
		ConfigLocation &operator=(const ConfigLocation &other);

		const std::string	&getUri(void) const;
		bool				inLocation(const std::string& decoded_uri) const;
};

#endif