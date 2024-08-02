/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 11:57:15 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/02 11:30:52 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "main.hpp"

class server;

class user
{
    private:

                server&             _server;
                int                 _clientFd;
                bool                _inChannel;
                int                 _idx;
                std::string         _name;
                std::string         _nickname;
                std::string         _currChannel;

    public:

                user(server & srv, int clientFD);
                ~user();

                bool            isValidName(void);
                bool            checkUserList(void);

                void            userName(void);
                void            join(void);
                void            info(void);
                void            who(void);
                void            nick(void);
                void            msg(void);
                void            quit(void);
                void            help(void);
                
                int             getClientFd(void);
                int             getIdx(void);
                std::string     getName(void);
                std::string     getNick(void);
                std::string     getCurrChannel(void);

                void            setIdx(int idx);

                class NotValidUserName : public std::exception
                {
                        virtual const char* what() const throw();
                };

                class NotValidNickName : public std::exception
                {
                        virtual const char* what() const throw();       
                };
};

#endif