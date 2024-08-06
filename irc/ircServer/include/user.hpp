/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 11:57:15 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/06 15:22:32 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "main.hpp"

class server;
class channel;

class user
{
    private:

                server&                 _server;
                int                     _clientFd;
                bool                    _inChannel;
                int                     _idx;
                std::string             _name;
                std::string             _username;
                std::string             _nickname;
                std::string             _currChannel;
                bool                    _opChannel;
                

    public:

                user(server & srv, int clientFd, std::vector<std::string> command);
                ~user();



                bool            isValidName(std::string name);
                bool            isValidChannelName(std::string name);
                bool            checkUserList(void);
                bool            checkChannel(void);
                int             checkChannel2(void);

                void            userName(void);
                void            join(void);
                void            nick(void);
                void            msg(void);
 
                void            info(void);
                void            who(void);
                void            list(void);
                void            quit(void);
                void            help(void);
                void            leave(void);

                void            speCommandOp(void);
                
                int             getClientFd(void);
                int             getIdx(void);
                std::string     getName(void);
                std::string     getNick(void);
                std::string     getCurrChannel(void);
                bool            getInChannel(void);
                bool            getOpChannel(void);

                void            setIdx(int idx);
                void            setOpchannel(bool op);
                void            setNickname(std::string nickname);
                void            registerChannel(std::string name, channel * channel);
                
                channel*        channelUser[MAXCHANNEL];
                channel*        getChannelByIdx(int idx);
                channel*        getChannelByName(std::string name); 


                class NotValidUserName : public std::exception
                {
                        virtual const char* what() const throw();
                };

                class NotValidNickName : public std::exception
                {
                        virtual const char* what() const throw();       
                };

                class NotValidChannelName : public std::exception
                {
                        virtual const char* what() const throw();
                };
};

// template <typename T> std::string toStr(T tmp)
// {
//     std::ostringstream out;
//     out << tmp;
//     return out.str();
// }

#endif