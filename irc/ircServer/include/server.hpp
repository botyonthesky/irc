/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/26 19:42:05 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <netdb.h>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <climits>
#include <vector>

#define PORT 4242
#define MAXCLIENT 10
#define MAXCHANNEL 3

class server
{
    private:
        
        struct sockaddr_in          _sa;
        struct sockaddr_storage     _clienAddr;
        socklen_t                   _addrSize;
        int                         _socketFd;
        int                         _clientFd;
        int                         _status;
        int                         _bytesRead;
        int                         _nbClient;
        std::string                 _loginClient[MAXCLIENT];
        std::string                 _nickClient[MAXCLIENT];
        std::string                 _channel[MAXCHANNEL];
        std::vector<std::string>    _command; 
        
    public:
        
        server();
        ~server();

        void    run(void);
        void    initServer(void);
        void    initSocket(void);
        void    initBind(void);
        void    initListen(void);
        void    readingClient(std::string loginClient);
        void    handleClient(int clientFd);
        void    manageMsg(std::string input);
        void    manageInput(std::string input);
        void    onlyOne(std::string input);
        void    help(void);
        void    nick(void);
        void    user(void);
        void    quit(void);
        void    who(void);

        void    sendMessage(std::string input);
        void    printInfoNewUser(int clientFd);

        
        class socketFdError : public std::exception
        {
            virtual const char* what() const throw();  
        };

        class bindError : public std::exception
        {
            virtual const char* what() const throw(); 
        };

        class listenError : public std::exception
        {
            virtual const char* what() const throw();
        };

        class clientFdError : public std::exception
        {
            virtual const char* what() const throw();  
        };

        class recvError : public std::exception
        {
            virtual const char* what() const throw();  
        };

        class sendError : public std::exception
        {
            virtual const char* what() const throw();
        };
};


template <typename T> std::string toStr(T tmp)
{
    std::ostringstream out;
    out << tmp;
    return out.str();
}

#endif