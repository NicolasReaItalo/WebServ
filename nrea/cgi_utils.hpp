/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_utils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 17:17:29 by nrea              #+#    #+#             */
/*   Updated: 2024/09/30 17:28:44 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_UTILS_HPP
#define CGI_UTILS_HPP

#include <map>
#include <iostream>
#include <string>
#include <vector>


char **MapToEnv(std::map<std::string,std::string> const & map);
std::string	detect_cgi(std::string const &uri, std::vector<std::string> const &cgi_ext);
void stripPathInfo(std::map<std::string,std::string> &headerAttributes, std::string cgi);
#endif
