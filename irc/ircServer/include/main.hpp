/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 13:07:04 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/01 10:56:52 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <poll.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <string>
#include <netdb.h>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <climits>
#include <vector>
#include <cstring> 

#include "server.hpp"
#include "user.hpp"
#include "channel.hpp"
#include "utils.hpp"

#define MAXCLIENT 10
#define MAXCHANNEL 3
#define PORT 4242


#endif 