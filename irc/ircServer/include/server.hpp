/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/01 09:57:38 by botyonthesk      ###   ########.fr       */
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
        std::vector<struct pollfd>  _pollFds;
        int                         _nbClient;
        int                         _idxClient[MAXCLIENT];
        user*                        _userN[MAXCLIENT];
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
        void    initPoll(void);
        void    waitingClient(void);
        void    readingClient(int clientFD);
        void    handleClient(int clientFd);
        void    removeUser(int clientFd);
        void    manageMsg(int clientFd, std::string input);
        void    parsingMsg(user * user, std::string input);
        void    manageInput(user * user, std::string input);
        void    onlyOne(user * user, std::string input);
        // void    help();
        // void    nick(void);
        // void    userName(void);
        void    quit(user *user);
        void    who();

        void    sendMessage(int clientFd, std::string message);
        void    parsingCommand(std::string input);
        void    printInfoNewUser(user *user);
        void    printInfoUsers();

        int                         getNbClient(void);
        int                         getNbChannel(void);
        std::vector<std::string>    getCommand(void);

        std::string                 loginClient[MAXCLIENT];
        
        class initError : public std::exception
        {
            public : 
                explicit initError(const std::string& error);
                virtual const char* what() const throw();
            
            private : 
                char       _error[100];
        };
};


template <typename T> std::string toStr(T tmp)
{
    std::ostringstream out;
    out << tmp;
    return out.str();
}

#endif