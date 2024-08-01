/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:38:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/31 11:45:58by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"

server::server() : _nbClient(0)
{
    // std::cout << "Server default constructor" << std::endl;
}

server::~server()
{
    close(_socketFd);
    for (size_t i = 0; i < _pollFds.size(); i++)
        close(_pollFds[i].fd);
    std::cout << "Server destructor" << std::endl;

}

const char* server::initError::what() const throw()
{
    return (_error);
}

server::initError::initError(const std::string& error)
{
    std::strncpy(_error, "Error on : ", 99);
    std::strncat(_error, error.c_str(), 99 - std::strlen(_error));
    _error[99] = '\0';

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
    {
        initError ex("socket fd");
        throw ex;
    }
}

void    server::initBind()
{
    _status = bind(_socketFd, (struct sockaddr *)&_sa, sizeof(_sa));
    if (_status != 0)
    {
        close(_socketFd);
        initError ex("bind");
        throw ex;
    }
}

void    server::initListen()
{
    _status = listen(_socketFd, 20);
    if (_status != 0)
    {
        close(_socketFd);
        initError ex("listen");
        throw ex;
    }
}

void    server::initPoll()
{
    struct pollfd serverPollFd;
    serverPollFd.fd = _socketFd;
    serverPollFd.events = POLLIN;
    _pollFds.push_back(serverPollFd);
}
void    server::quit(user * user)
{
    std::cout << "Now closing client : " << user->getName() << std::endl;
    close(user->getClientFd());
}

// void    server::who()
// {
//     std::cout << "You are actually in the channel : " << _channel << std::endl;
//     std::cout << "there is actually " << _nbClient << " client : " << std::endl;
//     for (int i = 0; i < _nbClient; i++)
//     {
//         std::cout << "Name : " << _loginClient[i] << ", nickname : "
//         << _nickClient[i] << std::endl;   
//     }
// }
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
    
    // std::cout << "parsing - >" << std::endl;
    input = trim_and_reduce_spaces(input);
    // std::cout << input << std::endl;
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


void    server::printCmd()
{
    for (std::vector<std::string>::iterator it = _command.begin(); it != _command.end(); it++)
    {
        std::cout << *it << std::endl;
    }
}

void    server::manageInput(user * user, std::string input)
{
    parsingCommand(input);
    // printCmd();
    int i = 0;
    std::string call[5] = {"/nick", "/user", "/quit", "/join", "/msg"};
    void (user::*ptr[5])() = {&user::nick, &user::userName, &user::quit, &user::join, &user::msg};
    while (i < 5)
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
        case 4 :
            {
                (user->*ptr[4])();
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

void    server::sendMessage(int clientFd, std::string message)
{
    int msgSize = message.size();
    int bytesSend = send(clientFd, message.c_str(), msgSize, 0);
    if (bytesSend == -1)
    {
        initError ex("send");
        throw ex;
    }
    else if (bytesSend != msgSize)
    {
        std::cout << "Only partial message received by client socket : " << _clientFd
        << ", bytes send = " << bytesSend << std::endl;
    }
}

void    server::manageMsg(int clientFd, std::string input)
{
    // std::cout << "manage msg" << std::endl;
    user* currUser = NULL;
    for(int i = 0; i < _nbClient; i++)
    {
        if (_userN[i]->getClientFd() == clientFd)
        {
            currUser = _userN[i];
            break;
        }
    }
    if (currUser != NULL)
        parsingMsg(currUser, input);
    else
        std::cout << "currUser is null" << std::endl;
    sendMessage(clientFd, "Got ya!");
}
void    server::parsingMsg(user * user, std::string input)
{
    size_t sepa = 0;
    sepa = input.find(" ");
    if (sepa != std::string::npos)
        manageInput(user, input);
    else
        onlyOne(user, input);
}

void    server::readingClient(int clientFd)
{
    char buff[BUFSIZ] = {0};
    _bytesRead = recv(clientFd, buff, BUFSIZ, 0);
    std::string input = buff;
    if (_bytesRead == -1)    
    {
        initError ex("recv");
        throw ex;
    }
    else if (_bytesRead == 0)
    {
        std::cout << "Client disconnected, fd : " << clientFd << std::endl;
        close(clientFd);
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].fd == clientFd)
            {
                _pollFds.erase(_pollFds.begin() + i);
                break;
            }
        }
        for (int i = 0; i < _nbClient; i++)
        {
            if (_userN[i]->getClientFd() == clientFd)
            {
                delete _userN[i];
                for(int j = i; j < _nbClient - 1; j++)
                    _userN[j] = _userN[j + 1];
                // _userN.erase(_userN.begin() + i);
                _nbClient--;
                break;
            }
        }
    }
    else
    {
        // std::cout << "waiting msg" << std::endl;
        buff[_bytesRead] = '\0';
        std::string input = buff;
        manageMsg(clientFd, input);
    }
}

void    server::handleClient(int clientFd)
{
    if (_nbClient >= MAXCLIENT)
    {
        std::cerr << "Max client reach" << std::endl;
        return ;
    }
    _clientFd = clientFd;
    user *newUser = new user(*this, clientFd);
    printInfoNewUser(newUser);
    _userN[_nbClient] = newUser;
    // printInfoUsers();
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
        initPoll();
        
        _addrSize = sizeof(_clienAddr);
        
        waitingClient();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void    server::waitingClient()
{
    std::cout << "Waiting for client connection" << std::endl << std::endl;
    while (true) 
    {
        int pollCount = poll(&_pollFds[0], _pollFds.size(), -1);
        if (pollCount == -1)
        {
            initError ex("poll");
            throw ex;
        }
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                if (_pollFds[i].fd == _socketFd)
                {
                    _clientFd = accept(_socketFd, (struct sockaddr *)&_clienAddr, &_addrSize);
                    if (_clientFd == -1)
                    {
                        initError ex("accept");
                        throw ex;
                    }
                    struct pollfd clientPollFd;
                    clientPollFd.fd = _clientFd;
                    clientPollFd.events = POLLIN;
                    _pollFds.push_back(clientPollFd);
                    std::cout << "New client connected, fd : " << _clientFd << std::endl;
                    handleClient(_clientFd);
                    _nbClient++;
                }
                else
                {
                    readingClient(_pollFds[i].fd);
                }
            }
        }

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

std::vector<std::string>  server::getLogin()
{
    return (_loginClient);
}

void    server::setLogin(std::string login)
{
    // std::cout << "set login : " << login << std::endl;
    _loginClient.push_back(login);
}
void    server::delUserList(user * user)
{
    // std::vector<std::string> tmp = this->_loginClient;
    if (_loginClient.size() == 1)
        _loginClient.pop_back();
    else
    {
        for (std::vector<std::string>::iterator it = _loginClient.begin(); it != _loginClient.end(); it++)
        {
            if (*it == user->getName())
            {
                _loginClient.erase(it);
                // printLoginList();
                break;
            }
        }
    }
}

void    server::updateLoginList(std::string old, std::string login)
{
    // std::vector<std::string> tmp = this->_loginClient;
    if (_loginClient.size() == 1)
        _loginClient[0] = login;
    else
    {
        for (std::vector<std::string>::iterator it = _loginClient.begin(); it != _loginClient.end(); it++)
        {
            if (*it == old)
            {
                _loginClient.erase(it);
                _loginClient.push_back(login);
                printLoginList();
                break;
            }
        }
    }
}


void    server::printLoginList()
{
    std::cout << "print list " << std::endl;
    std::vector<std::string> tmp = _loginClient;
    for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
    {
        std::cout << *it << std::endl;
    }
}

void   server::printInfoNewUser(user *user)
{
    std::cout << "there is a new client -> " << std::endl;
    std::cout << "Socket client : " <<  user->getClientFd() << std::endl
            << "Login client : "  << user->getName() << std::endl 
            << "Nickname client : " << user->getNick() << std::endl << std::endl;
}

void   server::printInfoUsers()
{
    for (int i = 0; i < _nbClient; i++)
    {
        std::cout << "Socket client : " << _userN[i]->getClientFd() << std::endl
            << "Login client : "  << _userN[i]->getName() << std::endl 
            << "Nickname client : " << _userN[i]->getNick() << std::endl << std::endl;
    }
}