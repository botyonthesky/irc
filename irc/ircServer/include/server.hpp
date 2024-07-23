/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/23 17:20:25 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <netdb.h>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4242

class server
{
    private:
        
        struct sockaddr_in      _sa;
        struct sockaddr_storage _clienAddr;
        socklen_t               _addrSize;
        int                     _socketFd;
        int                     _clientFd;
        int                     _status;
        int                     _bytesRead;
        
        
    public:
        
        server();
        ~server();

        void    run(void);
        void    initServer(void);
        void    initSocket(void);
        void    initBind(void);
        void    readingClient(void);
        void    handleClient(int clientFd);
        
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




#endif