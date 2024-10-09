/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_errors.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 14:10:54 by nrea              #+#    #+#             */
/*   Updated: 2024/09/17 10:07:26 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_ERRORS_HPP
#define HTTP_ERRORS_HPP

// Codes d'erreur HTTP 1.1
#define HTTP_STATUS_CONTINUE             "100" // Continue
#define HTTP_STATUS_SWITCHING_PROTOCOLS  "101" // Switching Protocols
#define HTTP_STATUS_OK                   "200" // OK
#define HTTP_STATUS_CREATED              "201" // Created
#define HTTP_STATUS_ACCEPTED             "202" // Accepted
#define HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION "203" // Non-Authoritative Information
#define HTTP_STATUS_NO_CONTENT           "204" // No Content
#define HTTP_STATUS_RESET_CONTENT        "205" // Reset Content
#define HTTP_STATUS_PARTIAL_CONTENT      "206" // Partial Content
#define HTTP_STATUS_MULTIPLE_CHOICES     "300" // Multiple Choices
#define HTTP_STATUS_MOVED_PERMANENTLY    "301" // Moved Permanently
#define HTTP_STATUS_FOUND                "302" // Found
#define HTTP_STATUS_SEE_OTHER            "303" // See Other
#define HTTP_STATUS_NOT_MODIFIED         "304" // Not Modified
#define HTTP_STATUS_USE_PROXY            "305" // Use Proxy
#define HTTP_STATUS_TEMPORARY_REDIRECT   "307" // Temporary Redirect
#define HTTP_STATUS_PERMANENT_REDIRECT   '308' // Permanent Redirect
#define HTTP_STATUS_BAD_REQUEST          "400" // Bad Request
#define HTTP_STATUS_UNAUTHORIZED         "401" // Unauthorized
#define HTTP_STATUS_PAYMENT_REQUIRED     "402" // Payment Required
#define HTTP_STATUS_FORBIDDEN            "403" // Forbidden
#define HTTP_STATUS_NOT_FOUND            "404" // Not Found
#define HTTP_STATUS_METHOD_NOT_ALLOWED   "405" // Method Not Allowed
#define HTTP_STATUS_NOT_ACCEPTABLE       "406" // Not Acceptable
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED "407" // Proxy Authentication Required
#define HTTP_STATUS_REQUEST_TIMEOUT      "408" // Request Timeout
#define HTTP_STATUS_CONFLICT             "409" // Conflict
#define HTTP_STATUS_GONE                 "410" // Gone
#define HTTP_STATUS_LENGTH_REQUIRED      "411" // Length Required
#define HTTP_STATUS_PRECONDITION_FAILED  "412" // Precondition Failed
#define HTTP_STATUS_PAYLOAD_TOO_LARGE    "413" // Payload Too Large
#define HTTP_STATUS_URI_TOO_LONG         "414" // URI Too Long
#define HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE "415" // Unsupported Media Type
#define HTTP_STATUS_RANGE_NOT_SATISFIABLE "416" // Range Not Satisfiable
#define HTTP_STATUS_EXPECTATION_FAILED   "417" // Expectation Failed
#define HTTP_STATUS_IM_A_TEAPOT          "418" // I'm a teapot (RFC 2324)
#define HTTP_STATUS_MISDIRECTED_REQUEST  "421" // Misdirected Request
#define HTTP_STATUS_UNPROCESSABLE_ENTITY "422" // Unprocessable Entity
#define HTTP_STATUS_LOCKED               "423" // Locked
#define HTTP_STATUS_FAILED_DEPENDENCY    "424" // Failed Dependency
#define HTTP_STATUS_UPGRADE_REQUIRED     "426" // Upgrade Required
#define HTTP_STATUS_PRECONDITION_REQUIRED "428" // Precondition Required
#define HTTP_STATUS_TOO_MANY_REQUESTS    "429" // Too Many Requests
#define HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE "431" // Request Header Fields Too Large
#define HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS "451" // Unavailable For Legal Reasons
#define HTTP_STATUS_INTERNAL_SERVER_ERROR "500" // Internal Server Error
#define HTTP_STATUS_NOT_IMPLEMENTED      "501" // Not Implemented
#define HTTP_STATUS_BAD_GATEWAY          "502" // Bad Gateway
#define HTTP_STATUS_SERVICE_UNAVAILABLE  "503" // Service Unavailable
#define HTTP_STATUS_GATEWAY_TIMEOUT      "504" // Gateway Timeout
#define HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED "505" // HTTP Version Not Supported
#define HTTP_STATUS_VARIANT_ALSO_NEGOTIATES "506" // Variant Also Negotiates
#define HTTP_STATUS_INSUFFICIENT_STORAGE "507" // Insufficient Storage
#define HTTP_STATUS_LOOP_DETECTED        "508" // Loop Detected
#define HTTP_STATUS_NOT_EXTENDED         "510" // Not Extended

#endif
