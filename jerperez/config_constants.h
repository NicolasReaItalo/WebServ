/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_constants.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 15:17:07 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_CONSTANTS_H
# define CONFIG_CONSTANTS_H

//
# define CF_KNOWN_DIRECTIVES "autoindex alias error_page index limit listen \
cgi client_body_path client_max_body_size location root return server_name"
//
# define CF_DFLT_AUTOINDEX "off"
# define CF_DFLT_ROOT "/"
# define CF_DFLT_LIMIT "GET POST DELETE"
# define CF_DFLT_CLIENT_BODY_PATH "/var/www/default_upload_path"
# define CF_DFLT_CLIENT_MAX_BODY_SIZE "500000"
//
# define CF_ERRDPARAM 2
# define CF_ERRDPARAM_MSG "invalid parameters for mandatory directive"
# define CF_ERRNOTB 3
# define CF_ERRNOTB_MSG "expected block directive"
# define CF_ERRNOTS 4
# define CF_ERRNOTS_MSG "expected server directive"
# define CF_ERRNOTL 5
# define CF_ERRNOTL_MSG "expected location"
# define CF_ERRNOTD 6
# define CF_ERRNOTD_MSG "expected simple directive"
# define CF_ERRNOURI 7
# define CF_ERRNOURI_MSG "no uri"
//DIRECTIVE
# define CF_ERRDUNKNOWN 8
# define CF_ERRDUNKNOWN_MSG "unknown directive name"
# define CF_ERRDNONAME 9
# define CF_ERRDNONAME_MSG "missing directive name"
# define CF_ERRDBADPNUM 10
# define CF_ERRDBADPNUM_MSG "invalid number of parameters"
# define CF_ERRDMISSING 11
# define CF_ERRDMISSING_MSG "mandatory directive missing"
//PARSING
# define CF_ERRDNOTCLOSED 21
# define CF_ERRDNOTCLOSED_MSG "directive missing `;'"
# define CF_ERRDBNOARGS 22
# define CF_ERRDBNOARGS_MSG "block directive with no arguments"
# define CF_ERRDSIMPLE 23
# define CF_ERRDSIMPLE_MSG "invalid simple directive"
# define CF_ERRDBLOCK 24
# define CF_ERRDBLOCK_MSG "invalid block directive"
# define CF_ERRDBADTOKEN 127
# define CF_ERRDBADTOKEN_MSG "invalid token"
//
# define CF_ERRNOTHING 30
# define CF_ERRNOTHING_MSG "no server"
// Change to 1 to enable debug
# ifndef CF_DEBUG
#  define CF_DEBUG 0
# endif

#endif
