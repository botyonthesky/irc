/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 17:16:40 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/08 14:06:39 by botyonthesk      ###   ########.fr       */
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
        int                 _idxUser;
        int                 _nbUsers;
        user*               _userN[MAXCLIENT];
        std::string         _nameOperator; 

    public:
    
    
        channel(user * user, std::string name);
        ~channel();

        // int             getIdxByUserName(std::string name);
        bool            isValidChannelName(std::string name);
        void            majIdxUserChannel(void);
        std::string     getName(void);
        int             getIdx(void);
        int             getIdxUser(void);
        int             getNbUser(void);
        user*           getUserN(int idx);
        int             getIdxUserByName(std::string name);

        void            setIdxUser(int idx);
        void            setIdx(int idx);
        void            setNbUser(int x);
        void            setUserN(user * user, int idx);
        void            delUserN(int idx);
        

        
};






#endif