/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 10:52:33 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 14:02:06 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "BlockSimpleConfig.hpp"

class LocationConfig : public BlockSimpleConfig
{
	private:
		std::string	_uri;
	public:
		LocationConfig(void) {};
		LocationConfig(const std::string &uri) : _uri(uri) {};
		~LocationConfig(void) {};
		LocationConfig(const LocationConfig &other);
		LocationConfig &operator=(const LocationConfig &other);

		const std::string	&getUri(void) const;
		bool				inLocation(const std::string& decoded_uri);
		// void				setUri(const std::string& uri);
};

#endif