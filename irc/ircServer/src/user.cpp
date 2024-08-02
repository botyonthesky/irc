/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:03:04 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/02 11:41:03 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"
#include "../include/server.hpp"

user::user(server& srv, int clientFd) : _server(srv), _clientFd(clientFd), _inChannel(false)
{
    std::cout << "User construct" << std::endl;
    std::ostringstream oss;
    oss << "GuestN_" << clientFd;
    _name = oss.str();
    _nickname = "undefined";
    _currChannel = "No channel";
    _server.setLogin(_name);
}

user::~user()
{
    _server.delUserList(this);
    std::cout << "User destruct of : " << this->_name << std::endl;
    
}


bool    user::isValidName()
{
    std::string name = _server.getCommand()[1];
    if (!isalpha(name[0]))
        return (false);
    return (true);
}

void    user::help()
{
    std::cout <<    "\"/info\"                     display your information\n"
                    "\"/nick [nick_name]\"         change your nickname\n"
                    "\"/user [login]\"             change your login\n"
                    "\"/join [channel]\"           join channel\n"
                    "/leave                      leave current channel\n"
                    "/quit                       quit irc\n"
                    "/who                        list of users in current channel\n"
                    "\"/msg [login] [msg]\"        submit msg to login\n"
                    "/list                       list of channel\n"
                    "[msg]                       send msg to current channel" << std::endl << std::endl;
}

void    user::info()
{
    std::cout << "Your user name is : " << _name << ", your nick name is : "
    << _nickname << std::endl; 
    if (!_inChannel)
        std::cout << "You re not in any channel right now !" << std::endl;
    else
        std::cout << "You re in the channel : " << _currChannel << std::endl;  
}
void    user::nick()
{
    try
    {
        if (!isValidName())
        {
            _server.sendMessage(_clientFd, "IRC", "Not valid nickname");
            throw NotValidNickName();
        }
        std::cout << "Your nickname was : " << _nickname
        << " its now : " << _server.getCommand()[1] << std::endl;
        _nickname = _server.getCommand()[1];
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void   user::userName()
{
    try
    {
        if (!isValidName())
        {
            _server.sendMessage(_clientFd, "IRC", "Not valid username");
            throw NotValidUserName();
        }
        std::cout << "user ->" << std::endl;
        std::cout << "You username was : " << _name
        << " its now : " << _server.getCommand()[1] << std::endl;
        _server.updateLoginList(_name, _server.getCommand()[1]);
        _name = _server.getCommand()[1]; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    user::join()
{
    // std::cout << "join ->" << std::endl;
    channel *newChannel = new channel(_server.getCommand()[1]);
    (void)newChannel;
}
bool    user::checkUserList()
{
    // std::cout << " check user list " << std::endl;
    std::vector<std::string> tmp = _server.getLogin();
    if (tmp.size() == 1)
        return (false);
    else
    {
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
        {
            if (_server.getCommand()[1] == *it)
                return (true);
        }
    }   
    return (false);
    
}

void    user::msg()
{
    try
    {
        if (!checkUserList())
            throw NotValidUserName();
        int fdToSend = _server.findFdClient(_server.getCommand()[1]);
        _server.sendMessage(fdToSend, this->getName(), _server.getCommand()[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    user::quit()
{
    std::cout << _name << " have quit the server" << std::endl;
    user::~user();
    // close(_clientFd);
}

void    user::who()
{
    if (!_inChannel)
    {
        std::cout << "You re not in any channel right now !" << std::endl;
        std::cout << "There is actually " << _server.getNbClient() << " client(s) in the server." << std::endl;
    }
    else
    {
        std::cout << "You are actually in the channel : " << _currChannel << std::endl;
        std::cout << "there is actually " << _server.getNbClient() << " client(s) in this channel -> " << std::endl;
    }
    std::vector<std::string> tmp = _server.getLogin();
    if (tmp.size() == 1) 
        std::cout << "User : " << this->_name << std::endl;
    else
    {
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
        {
            std::cout << "User : " << *it << std::endl;
        }
    }
}

int             user::getClientFd(void)
{
    return (_clientFd);    
}
int            user::getIdx(void)
{
    return (_idx);
}

void        user::setIdx(int idx)
{
    _idx = idx;
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
const char*     user::NotValidUserName::what() const throw()
{
    return ("The username is not valid !");
}


const char*     user::NotValidNickName::what() const throw()
{
    return ("The nickname is not valid !");
}