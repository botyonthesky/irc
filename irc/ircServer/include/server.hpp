/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:37:25 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/09 18:09:17 by botyonthesk      ###   ########.fr       */
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
        
        int                         _nbChannel;

        std::vector<std::string>    _command; 

        
    public:
        
        server();
        ~server();

        channel*                    channelId[MAXCHANNEL];
        std::vector<std::string>    nicknameClient;

        void    run(void);
        void    initServer(void);
        void    initSocket(void);
        void    initBind(void);
        void    initListen(void);
        void    initPoll(void);

        void    closeFd(int clientFd);
        void    recepMsg(std::string input, int clientFd, bool info);
        void    waitingClient(void);
        void    readingClient(int clientFd);
        bool    manageUserInfo(int clientFd, std::string input);
        bool    manageUser(int clientFd, std::vector<std::string> command);
        bool    manageNick(int clientFd, std::string nickname);

        bool    isValidUsername(std::string username);
        bool    isValidNickname(std::string nickname);
        void    infoRequired(int clientFd);
        void    handleClient(int clientFd);
        void    infoClient(int clientFd);
        void    receptInfo(std::string input, int clientFd);
        void    readingInfo(int clientFd);

        user*   getUserByFd(int clientFd); 
        void    onlyOne(user * user, std::string input);
        void    manageMsg(int clientFd, std::string input);
        void    sendMessage(int clientFd, std::string from, std::string message);
        void    parsingMsg(user * user, std::string input);
        void    manageInput(user * user, std::string input);
        void    msgToCurrent(user * user, std::string input);
       
        void    updateLoginList(std::string old, std::string login);
        void    updateNicknameList(std::string old, std::string nickname);

        void    printLoginList(void);
        void    delUserList(user * user);
        void    removeUser(int clientFd);
        void    sendInputToChanFromUser(channel * channel, user * user, std::string input);
        int     getFdClientByNick(std::string nickname);
        
        void    quit(user *user);

        void    parsingCommand(std::string input);
        void    printCmd(void);

        void    printInfoNewUser(user *user);
        void    printInfoUsers(void);

        std::vector<std::string>    parsingIntoVector(std::string input);

        void                        checkChannel(std::string currChannel);        
        void                        printChannelInfo(void);
        void                        printChannelInfoByChannel(std::string channelName);

        // void                        decremChannelNbUser(std::string currChannel);
        void                        getNbUserInChannel(std::string currChannel);
        user*                       getUserByNickname(std::string nickname);

        int                         getNbClient(void);
        int                         getSocketfd(void);
        int                         getNbChannel(void);
        std::vector<std::string>    getCommand(void);
        std::vector<std::string>    getLogin(void);

        void                        setLogin(std::string login);
        void                        setNbChannel(int nb);


        
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