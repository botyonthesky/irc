/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/30 16:15:48 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../include/main.hpp"

#define MAXCLIENT 10
#define MAXCHANNEL 3
#define PORT 4242

class user;

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
        int                         _idxClient[MAXCLIENT];
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
        void    waitingClient(void);
        void    readingClient(user * user);
        void    handleClient(int clientFd);
        void    manageMsg(user * user, std::string input);
        void    manageInput(user * user, std::string input);
        void    onlyOne(user * user, std::string input);
        // void    help();
        // void    nick(void);
        // void    userName(void);
        void    quit(user *user);
        void    who();

        void    sendMessage(std::vector<std::string> command);
        void    parsingCommand(std::string input);
        void    printInfoNewUser(user *user);

        int                         getNbClient(void);
        int                         getNbChannel(void);
        std::vector<std::string>    getCommand(void);

        std::string                 loginClient[MAXCLIENT];
        
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