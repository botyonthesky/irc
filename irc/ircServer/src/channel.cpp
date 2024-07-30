/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:18:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/30 17:43:44 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"

channel::channel(std::string name)
{
    try
    {
       if (!isValidChannelName(name))
            throw NotValidChannelName();   
        _name = name;
        std::cout << "you ve join the channel : " << _name << std::endl;
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


const char* channel::NotValidChannelName::what() const throw()
{
    return ("The channel name is not valid\nUsage : ('#' | '&') <chstring>");
}