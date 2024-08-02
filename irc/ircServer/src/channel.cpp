/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:18:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/02 17:46:52 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"

channel::channel(user * user, std::string name) : _nbUsers(0)
{
    try
    {
       if (!isValidChannelName(name))
            throw NotValidChannelName();
        _name = name;
        _nbUsers++;
        _userN[_nbUsers] = user;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}



channel::~channel()
{
}
bool    channel::isValidChannelName(std::string name)
{
    if (name[0] == '&' || name[0] == '#')
        return (true);
    return (false);
}
std::string     channel::getName()
{
    return (_name);
}
int         channel::getIdx(void)
{
    return (_idx);
}
void        channel::setIdx(int idx)
{
    _idx = idx;
}

int         channel::getNbUser()
{
    return (_nbUsers);
}

user*       channel::getUserN(int idx)
{
   return (_userN[idx]);
}
void        channel::setNbUser(int x)
{
    _nbUsers += x;
}
const char* channel::NotValidChannelName::what() const throw()
{
    return ("The channel name is not valid\nUsage : ('#' | '&') <chstring>");
}