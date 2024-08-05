/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/05 17:31:12 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../include/main.hpp"

#define MAXCLIENT 10
#define MAXCHANNEL 3
#define PORT 4242

class user;
class channel;

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
        user*                       _userN[MAXCLIENT];
        std::vector<std::string>    _loginClient;
        std::string                 _nickClient[MAXCLIENT];
        
        // channel*                    _channel[MAXCHANNEL];
        std::string                 _channelName[MAXCHANNEL];
        int                         _nbChannel;
        
        std::vector<std::string>    _command; 

        
    public:
        
        server();
        ~server();

        channel*                    channelId[MAXCHANNEL];

        void    run(void);
        void    initServer(void);
        void    initSocket(void);
        void    initBind(void);
        void    initListen(void);
        void    initPoll(void);
        
        void    waitingClient(void);
        void    readingClient(int clientFD);
        void    handleClient(int clientFd);
        void    infoClient(int i);

        
        void    onlyOne(user * user, std::string input);
        void    manageMsg(int clientFd, std::string input);
        void    sendMessage(int clientFd, std::string from, std::string message);
        void    parsingMsg(user * user, std::string input);
        void    manageInput(user * user, std::string input);
        void    msgToCurrent(user * user, std::string input);
       
        void    updateLoginList(std::string old, std::string login);
        void    printLoginList(void);
        void    delUserList(user * user);
        void    removeUser(int clientFd);
        void    findUser(channel * channel, user * user, std::string input);
        int     findFdClient(std::string user);
        
        void    quit(user *user);

        void    parsingCommand(std::string input);
        void    printCmd(void);

        void    printInfoNewUser(user *user);
        void    printInfoUsers(void);


        void                        checkChannel(std::string currChannel);        
        void                        printChannelInfo(void);
        void                        decremChannelNbUser(std::string currChannel);
        void                        getNbUserInChannel(std::string currChannel);

        int                         getNbClient(void);
        int                         getSocketfd(void);
        int                         getNbChannel(void);
        std::vector<std::string>    getCommand(void);
        std::vector<std::string>    getLogin(void);
        // channel*                    getChannel(int idx);

        void                        setLogin(std::string login);
        void                        setNbChannel(int nb);
        // void                        setChannel(channel * channel, int idx);


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