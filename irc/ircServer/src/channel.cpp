/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:18:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/06 13:16:39 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"

channel::channel(user * user, std::string name) : _nbUsers(0)
{
    try
    {
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

void        channel::setUserN(user * user, int idx)
{
    _userN[idx] = user;
}