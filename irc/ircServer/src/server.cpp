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

server::server() : _nbClient(0), _nbChannel(0)
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
    for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].fd == user->getClientFd())
            {
                _pollFds.erase(_pollFds.begin() + i);
                break;
            }
        }
        for (int i = 1; i <= _nbClient; i++)
        {
            if (_userN[i]->getClientFd() == user->getClientFd())
            {
                delete _userN[i];
                for(int j = i; j < _nbClient - 1; j++)
                    _userN[j] = _userN[j + 1];
                _nbClient--;
                break;
            }
        }
}

void   server::onlyOne(user * user, std::string input)
{
    if (input == "/quit")
        quit(user);
    int i = 0;
    std::string call[5] = {"/info", "/help", "/who", "/list", "/leave"};
    void (user::*ptr[5])() = {&user::info, &user::help, &user::who, &user::list, &user::leave};
    while (i < 5)
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
        case 3:
            {
                (user->*ptr[3])(); 
                break;
            }
        case 4:
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
        }
    }
}


void    server::parsingCommand(std::string input)
{
    input = trim_and_reduce_spaces(input);
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
    int i = 0;
    std::string call[4] = {"/nick", "/user", "/join", "/msg"};
    void (user::*ptr[4])() = {&user::nick, &user::userName, &user::join, &user::msg};
    while (i < 4)
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
                msgToCurrent(user, input);
                // if (user->getNick() == "undefined")
                //     std::cout << user->getName() << ": ";
                // else
                //     std::cout << user->getNick() << ": ";
                // std::cout << input << std::endl << std::endl;
                break;
            }
    }
}

void    server::printChannelInfo()
{
    std::cout << "each channel info" << std::endl;
    for (int i = 1; i <= _nbChannel; i++)
    {
        std::cout << channelId[i]->getName() << std::endl;
        std::cout << channelId[i]->getIdx() << std::endl;
        std::cout << channelId[i]->getNbUser() << std::endl;
    }
}


void    server::msgToCurrent(user * user, std::string input)
{
    if (!user->getInChannel())
    {
        std::cout << "Currently you aren't in any channel ! " << std::endl;
    }
    else
    {
        std::string channel = user->getCurrChannel();
        std::cout << channel << std::endl;
        (void)input;
        // printChannelInfo()
        // for (int i = 1; i <= _nbChannel; i++)
        // {
        //     if (channel == this->channelId
        //     {
        //         findUser(channelId[i], user, input);
        //         break ;
        //     }
        // }
    }
}
void    server::findUser(channel * channel, user * user, std::string input)
{
    for (int i = 0; i < channel->getNbUser(); i++)
    {
        if (channel->getUserN(i) == user)
            i++;
        else
            sendMessage(channel->getUserN(i)->getClientFd(), "", input);
    }
}
void    server::sendMessage(int clientFd, std::string from, std::string message)
{
    std::string userMsg;
    if (from.empty())
        userMsg = message;
    else
    {
        char sep = ' ';
        userMsg = from + sep + message;
    }
    int msgSize = userMsg.size();
    int bytesSend = send(clientFd, userMsg.c_str(), msgSize, 0);
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
    user* currUser = NULL;
    for(int i = 1; i <= _nbClient; i++)
    {
        if (_userN[i]->getClientFd() == clientFd)
        {
            currUser = _userN[i];
            break;
        }
    }
    if (currUser != NULL)
    {
        parsingMsg(currUser, input);
        if (input != "/quit")
            infoClient(currUser->getIdx());
    }
    else
        std::cout << "currUser is null" << std::endl;
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
        // std::cout << "Client disconnected, fd : " << clientFd << std::endl;
        close(clientFd);
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].fd == clientFd)
            {
                _pollFds.erase(_pollFds.begin() + i);
                break;
            }
        }
        for (int i = 1; i <= _nbClient; i++)
        {
            if (_userN[i]->getClientFd() == clientFd)
            {
                delete _userN[i];
                for(int j = i; j < _nbClient - 1; j++)
                    _userN[j] = _userN[j + 1];
                _nbClient--;
                break;
            }
        }
    }
    else
    {
        buff[_bytesRead] = '\0';
        std::string input = buff;
        manageMsg(clientFd, input);
    }
}

void    server::handleClient(int clientFd)
{
    _nbClient++;
    std::cout << "nb client " << _nbClient << std::endl;
    if (_nbClient >= MAXCLIENT)
    {
        std::cerr << "Max client reach" << std::endl;
        return ;
    }
    _clientFd = clientFd;
    user *newUser = new user(*this, clientFd);
    printInfoNewUser(newUser);
    newUser->setIdx(_nbClient);
    _userN[_nbClient] = newUser;
    infoClient(_nbClient);    
}

void    server::infoClient(int i)
{
    std::string info;
    std::string format = "$> ";
    std::string format2 = " ------------IRC------------[";
    std::string format3 = "]-------------->";
    info = format + _userN[i]->getName() + format2
    + _userN[i]->getCurrChannel() + format3;
    int msgSize = info.size();
    int byteS = send(_userN[i]->getClientFd(), info.c_str(), msgSize, 0);
    if (byteS == -1)
    {
        initError ex ("send");
        throw ex;
    }
    else if (byteS != msgSize)
    {
        std::cout << "Only partial message received by client socket : " << _clientFd
        << ", bytes send = " << byteS << std::endl;
    }
    // }   
}


int     server::findFdClient(std::string user)
{
   int fd;
    for (int i = 1; i <= _nbClient; i++)
    {
        if (_userN[i]->getName() == user)
            fd = _userN[i]->getClientFd();
    }
    return (fd);
}
void    server::run()
{
    try
    {
        initServer();
        initSocket();
        initBind();

        std::cout << "[ SERVER RUN ]" << std::endl;
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
                }
                else
                {
                    readingClient(_pollFds[i].fd);
                }
            }
        }
    }
}


void    server::delUserList(user * user)
{
    if (_loginClient.size() == 1)
        _loginClient.pop_back();
    else
    {
        for (std::vector<std::string>::iterator it = _loginClient.begin(); it != _loginClient.end(); it++)
        {
            if (*it == user->getName())
            {
                _loginClient.erase(it);
                break;
            }
        }
    }
}

void    server::updateLoginList(std::string old, std::string login)
{
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
    for (int i = 1; i <= _nbClient; i++)
    {
        std::cout << "Socket client : " << _userN[i]->getClientFd() << std::endl
            << "Login client : "  << _userN[i]->getName() << std::endl 
            << "Nickname client : " << _userN[i]->getNick() << std::endl << std::endl;
    }
}

int     server::getNbClient(void)
{
    return (_nbClient);
}
int     server::getNbChannel(void)
{
    return (_nbChannel);
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
    _loginClient.push_back(login);
}

int     server::getSocketfd()
{
    return (_socketFd);
}

void    server::setNbChannel(int i)
{
    _nbChannel += i;
}
// void        server::setChannel(channel * channel, int idx)
// {
//     _channel[idx] = channel;
// }

// channel*    server::getChannel(int idx)
// {
//     return (_channel[idx]);
// }