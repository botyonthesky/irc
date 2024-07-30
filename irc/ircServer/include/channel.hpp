/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:16:40 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/30 17:29:08 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "main.hpp"

class channel
{
    private:
        
        std::string         _name;
        

    
    public:
    
    
        channel(std::string name);
        ~channel();


        bool            isValidChannelName(std::string name);
        std::string     getName(void);
        
        class NotValidChannelName : public std::exception
        {
            virtual const char* what() const throw();
        };

        
};






#endif