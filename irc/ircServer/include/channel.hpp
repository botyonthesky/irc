/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:16:40 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/06 12:08:18 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "main.hpp"

class user;

class channel
{
    private:
        
        std::string         _name;
        int                 _idx;
        int                 _nbUsers;
        user*               _userN[MAXCLIENT]; 

    public:
    
    
        channel(user * user, std::string name);
        ~channel();


        bool            isValidChannelName(std::string name);
        std::string     getName(void);
        int             getIdx(void);
        int             getNbUser(void);
        user*           getUserN(int idx);

        void            setIdx(int idx);
        void            setNbUser(int x);
        void            setUserN(user * user, int idx);
        

        
};






#endif