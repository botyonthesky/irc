/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 11:57:15 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/29 14:39:31 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "main.hpp"

class user
{
    private:
    
            int                 _clientFd;
            int                 _idx;
            std::string         _name;
            std::string         _nickname;
            
             
        
    public:

            user(int clientFD);
            ~user();
            
            int             getClientFd(void);
            int             getIdx(void);
            std::string     getName(void);
            std::string     getNick(void);


            void        userName(void);
            // void        who(void);
            void        nick(void);
            void        quit(void);
};





#endif