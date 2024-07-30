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
}

void    server::initBind()
{
    _status = bind(_socketFd, (struct sockaddr *)&_sa, sizeof(_sa));
    if (_status != 0)
        throw bindError();
}
void    server::initListen()
{
    _status = listen(_socketFd, 20);
    if (_status != 0)
        throw listenError();
}





void    server::quit(user * user)
{
    std::cout << "Now closing client : " << user->getName() << std::endl;
    close(user->getClientFd());
    waitingClient();
}


void    server::who()
{
    std::cout << "You are actually in the channel : " << _channel << std::endl;
    std::cout << "there is actually " << _nbClient << " client : " << std::endl;
    for (int i = 0; i < _nbClient; i++)
    {
        std::cout << "Name : " << _loginClient[i] << ", nickname : "
        << _nickClient[i] << std::endl;   
    }
}
void   server::onlyOne(user * user, std::string input)
{
    if (input == "/quit")
    {
        quit(user);
        return ;
    }
    int i = 0;
    std::string call[3] = {"/info", "/help", "/who"};
    void (user::*ptr[3])() = {&user::info, &user::help, &user::who};
    while (i < 3)
    {
        if (input == call[i])
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
        case 2:
            {
                (user->*ptr[2])(); 
                break;
            }
        default :
        {
            if (user->getNick() == "undefined")
                std::cout << user->getName() << ": ";
            else
                std::cout << user->getNick() << ": ";
            std::cout << input << std::endl << std::endl;
            
        }
    }
}


void    server::parsingCommand(std::string input)
{
    // std::cout << "parsing input : " << input << std::endl;
    size_t start = 0;
    size_t end = input.find(" ");
    _command.clear();
    while (end != std::string::npos)
    {
        _command.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(" ", start);
    }
    _command.push_back(input.substr(start));
}

void    server::manageInput(user * user, std::string input)
{
    parsingCommand(input);
    // sendMessage(_command);
    int i = 0;
    std::string call[4] = {"/nick", "/user", "/quit", "/join"};
    void (user::*ptr[4])() = {&user::nick, &user::userName, &user::quit, &user::join};
    while (i < 3)
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
        case 3 :
            {
                (user->*ptr[3])();
                break;
            }
        default :
            {
                if (user->getNick() == "undefined")
                    std::cout << user->getName() << ": ";
                else
                    std::cout << user->getNick() << ": ";
                std::cout << input << std::endl << std::endl;
                break;
            }
    }
}
void    server::sendMessage(std::vector<std::string> command)
{
    std::cout << "------------------contenu de command-------------------" << std::endl;
    for (std::vector<std::string>::iterator it = command.begin(); it != command.end(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    if (command.empty())
        std::cout << " is empty" << std::endl; 
    std::cout << "------------------fin de command-------------------" << std::endl;
}

void    server::manageMsg(user * user, std::string input)
{
    size_t sepa = 0;
    sepa = input.find(" ");
    if (sepa != std::string::npos)
        manageInput(user, input);
    else
        onlyOne(user, input);
}

void    server::readingClient(user * user)
{
    std::cout << user->getName() << " is connected" << std::endl << std::endl;
    std::cout << "\"/help\" for info." << std::endl;
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
        user *newUser = new user(*this, clientFd);
        _idxClient[i] = i;
        loginClient[i] = newUser->getName();
        std::cout << "loginnnn : " << loginClient[i] << std::endl;
        printInfoNewUser(newUser);
        readingClient(newUser);
    }
}


int     server::getNbClient(void)
{
    return (_nbClient);
}
int     server::getNbChannel(void)
{
    int i = 0;
    return (i);
}
std::vector<std::string>   server::getCommand(void)
{
    return (_command);
}