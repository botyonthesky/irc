/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:03:04 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/30 15:28:58 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"
#include "../include/server.hpp"

user::user(server& srv, int clientFd) : _server(srv), _clientFd(clientFd), _inChannel(false)
{
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
std::string     user::getCurrChannel(void)
{
    return (_currChannel);
}

void   user::userName()
{
    std::cout << "user" << std::endl;
}

void    user::help()
{
    std::cout << "\"/nick [nick_name]\"         change your nickname\n"
                    "\"/user [login]\"             change your login\n"
                    "\"/join [channel]\"           join channel\n"
                    "/leave                      leave current channel\n"
                    "/quit                       quit irc\n"
                    "/who                        list of users in current channel\n"
                    "\"/msg [login] [msg]\"        submit msg to login\n"
                    "/list                       list of channel\n"
                    "[msg]                       send msg to current channel" << std::endl << std::endl;
}
void    user::nick()
{
    std::cout << "nick ->" << std::endl;
    

}
void    user::quit()
{
    std::cout << _name << " have quit the server" << std::endl;
    close(_clientFd);
}

void    user::who()
{
    if (!_inChannel)
        std::cout << "You re not in any channel right now !" << std::endl;
    else
    {
        std::cout << "You are actually in the channel : " << _currChannel << std::endl;
        std::cout << "there is actually " << _server.getNbClient() << " client(s) in this channel -> " << std::endl;
        for (int i = 0; i < _server.getNbClient(); i++)
        {
            std::cout << "Name : " << _server.loginClient[i] << ", nickname : " << std::endl;   
        }
    }
}







