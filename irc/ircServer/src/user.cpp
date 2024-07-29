/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:03:04 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/29 14:39:40 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"
// #include "../include/server.hpp"

user::user(int clientFd)
{
    _clientFd = clientFd;
    std::ostringstream oss;
    oss << "Guest N_" << clientFd;
    _name = oss.str();
    _nickname = "Undefined";
    std::cout << "User construct" << std::endl;
}

user::~user()
{
    std::cout << "User destruct" << std::endl;
}

int             user::getClientFd(void)
{
    return (_clientFd);    
}
int            user::getIdx(void)
{
    return (_idx);
}

std::string     user::getName(void)
{
    return (_name);
}

std::string     user::getNick(void)
{
    return (_nickname);
}

void   user::userName()
{
    std::cout << "user" << std::endl;
}


void    user::nick()
{
    std::cout << "nick ->" << std::endl;
    

}
void    user::quit()
{
    std::cout << _name << "have quit the server" << std::endl;
    close(_clientFd);
}

