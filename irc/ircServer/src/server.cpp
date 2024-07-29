/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:38:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/26 11:20:47y botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../include/server.hpp"
// #include "../include/user.hpp"
#include "../include/main.hpp"

server::server() : _nbClient(1)
{
    // std::cout << "Server default constructor" << std::endl;
}

server::~server()
{
    std::cout << "Server destructor" << std::endl;
}

const char* server::socketFdError::what() const throw()
{
    return ("Error on socket fd");
}

const char* server::bindError::what() const throw()
{
    return ("Error on bind");
}

const char* server::listenError::what() const throw()
{
    return ("Error on listen");
}

const char* server::clientFdError::what() const throw()
{
    return ("Error on cliend fd");
}

const char* server::recvError::what() const throw()
{
    return ("Error on recv");
}
const char* server::sendError::what() const throw()
{
    return ("Error on send");
}
void    server::initServer()
{
    _sa.sin_family = AF_INET;
    _sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    _sa.sin_port = htons(PORT);
}

void    server::initSocket()
{
    
    _socketFd = socket(_sa.sin_family, SOCK_STREAM, 0);
    if (_socketFd == -1)
        throw socketFdError();
    // std::cout << "Creation of server socket fd : " << _socketFd << std::endl;
}

void    server::initBind()
{
    _status = bind(_socketFd, (struct sockaddr *)&_sa, sizeof(_sa));
    // std::cout << "Status of bind : " << _status << std::endl;
    if (_status != 0)
        throw bindError();
}
void    server::initListen()
{
    // std::cout << "Listen on port : " << PORT << std::endl;
    _status = listen(_socketFd, 20);
    
    // std::cout << "Status of listen : " << _status << std::endl;
        
    if (_status != 0)
        throw listenError();
}

void    server::help()
{
    std::cout << "\"/nick [nick_name]\"         change your nickname\n"
                    "\"/user [login]\"             change your login\n"
                    "\"/join [channel]\"           join channel\n"
                    "/leave                      leave current channel\n"
                    "/quit                       quit irc\n"
                    "/who                        list of users in current channel\n"
                    "\"/msg [login] [msg]\"        submit msg to login\n"
                    "/list                       list of channel\n"
                    "[msg]                       send msg to current channel" << std::endl << std::endl;
}



void    server::quit(user * user)
{
    std::cout << "Now closing client : " << user->getName() << std::endl;
    close(user->getClientFd());
    waitingClient();
}


void    server::who()
{
    std::cout << "who" << std::endl;
}
void   server::onlyOne(user * user, std::string input)
{
    if (input == "/quit")
    {
        quit(user);
        return ;
    }
    int i = 0;
    std::string call[2] = {"/help", "/who"};
    void (server::*ptr[2])() = {&server::help, &server::who};
    while (i < 2)
    {
        if (input == call[i])
            break;
        i++;
    }
    switch (i)
    {
        case 0:
            {
                (this->*ptr[0])();
                break;
            }
        case 1:
            {
                (this->*ptr[1])(); 
                break;
            }
        default :
        {
            sendMessage(input);
            std::cout << "\"/help\" for info." << std::endl;
        }
    }
}



void    server::manageInput(user * user, std::string input)
{
    size_t start = 0;
    size_t end = input.find(" ");
    while (end != std::string::npos)
    {
        _command.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(" ", start);
    }
    _command.push_back(input.substr(start));
    int i = 0;
    std::string call[3] = {"/nick", "/user", "/quit"};
    void (user::*ptr[3])() = {&user::nick, &user::userName, &user::quit};
    while (i < 2)
    {
        if (_command[0] == call[i])
            break;
        i++;
    }
    switch (i)
    {
        case 0:
            {
                (user->*ptr[0])();
                break;
            }
        case 1:
            {
                (user->*ptr[1])(); 
                break;
            }
        case 2 :
            {
                (user->*ptr[2])();
                break;
            }
        default :
            {
                sendMessage(input);
                break;
            }
    }
}
void    server::sendMessage(std::string input)
{
    std::cout << input << std::endl;
}

void    server::manageMsg(user * user, std::string input)
{
    size_t sepa = 0;
    sepa = input.find(" ");
    if (sepa != std::string::npos)
    {
        manageInput(user, input);
    }
    else
    {
        onlyOne(user, input);
    }
}

void    server::readingClient(user * user)
{
    std::cout << user->getName() << " is connected" << std::endl << std::endl;
    _bytesRead = 1;
    
    while (_bytesRead >= 0)
    {
        char buff[BUFSIZ] = {0};
        _bytesRead = recv(user->getClientFd(), buff, BUFSIZ, 0);
        std::string input = buff;
        
        if (_bytesRead == -1)    
            throw recvError();
        else
        {
            std::string message = "Got ya !";
            int msgSize = message.size();
            int bytesSend;
            buff[_bytesRead] = '\0';
            bytesSend = send(user->getClientFd(), message.c_str(), msgSize, 0);
            if (bytesSend == -1)
                throw sendError();
            else if (bytesSend == msgSize)
            {
                manageMsg(user, input);
            }
            else
            {
                std::cout << "Only partial message received by client socket : " << _clientFd
                << ", bytes send = " << bytesSend << std::endl;
            }
        }
    }
    std::cout << "Now closing client fd : " << _clientFd << std::endl;
    close(_clientFd);
}

void   server::printInfoNewUser(user *user)
{
    std::cout << "there is a new client -> " << std::endl;
    std::cout << "Socket client : " <<  user->getClientFd() << std::endl
            << "Login client : "  << user->getName() << std::endl 
            << "Nickname client : " << user->getNick() << std::endl << std::endl;
}


void    server::run()
{
    try
    {
        initServer();
        initSocket();
        initBind();

        std::cout << "Connection socket to localhost, PORT : " << PORT << std::endl;
        std::cout << std::endl;

        initListen();
        
        _addrSize = sizeof(_clienAddr);
        
        waitingClient();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void    server::waitingClient()
{
    std::cout << "Waiting for client connection" << std::endl << std::endl;
    while (true) 
    {
        _clientFd = accept(_socketFd, (struct sockaddr *)&_clienAddr, &_addrSize);
        if (_clientFd == -1)
            throw clientFdError();
        handleClient(_clientFd);
        _nbClient++;
    }
}
void    server::handleClient(int clientFd)
{
    _clientFd = clientFd;
    

    for (int i = 0; i < _nbClient; i++)
    {
        user *newUser = new user(clientFd);
        _idxClient[i] = i;
        printInfoNewUser(newUser);
        readingClient(newUser);
    }
}
