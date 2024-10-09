/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_error_constants.h                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/09 16:16:20 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_ERROR_CONSTANTS_H
# define CONFIG_ERROR_CONSTANTS_H

//
# define CF_ERRDPARAM 2
# define CF_ERRNOTB 3
# define CF_ERRNOTS 4
# define CF_ERRNOTL 5
# define CF_ERRNOTD 6
# define CF_ERRNOURI 7
//DIRECTIVE
# define CF_ERRDUNKNOWN 8
# define CF_ERRDNONAME 9
# define CF_ERRDNOPARAM 10
# define CF_ERRDMISSING 11
//PARSING
# define PR_ERRDNOCONTEXT 20
# define PR_ERRDBADTOKEN 127
# define PR_ERRDNOTCLOSED 21
# define PR_ERRDBNOARGS 22
# define PR_ERRDSIMPLE 23
# define PR_ERRDBLOCK 24
# define PR_ERRDOPENFAIL 25

#endif
