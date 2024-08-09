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
void    server::closeFd(int clientFd)
{
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
void    server::quit(user * user)
{
    std::cout << "Now closing client : " << user->getUsername() << std::endl;
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
    if (input == "channel")
        printChannelInfo();
    if (input == "user")
        user->checkUserChannelList();
    int i = 0;
    std::string call[5] = {"/info", "/help", "/who", "/list", "/leave"};
    void (user::*ptr[5])() = {&user::info, &user::help, &user::who, &user::list, &user::leave};
    while (i < 5 && input != call[i])
        i++;
    if (i < 5)
        (user->*ptr[i])();
    else
        msgToCurrent(user, input);
}


void    server::manageInput(user * user, std::string input)
{
    parsingCommand(input);
    int i = 0;
    std::string call[5] = {"/nick", "/user", "/join", "/msg", "KICK"};
    void (user::*ptr[5])() = {&user::nick, &user::userName, &user::join, &user::msg, &user::kick};
    while (i < 5 && _command[0] != call[i])
        i++;
    if (i < 5)
        (user->*ptr[i])();
    else
        msgToCurrent(user, input);
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

void    server::msgToCurrent(user * user, std::string input)
{
    if (!user->getInChannel())
        sendMessage(user->getClientFd(), "IRC", "Currently you aren't in any channel !");
    else
    {
        std::string channelName = user->getCurrChannel();
        channel *curr = user->getChannelByName(channelName);
        findUser(curr, user, input);
    }
}
void    server::findUser(channel * channel, user * user, std::string input)
{
    if (channel->getNbUser() == 1)
        sendMessage(user->getClientFd(), "", "You re alone in this channel");
    else
    {
        int i = 1;
        while (i <= channel->getNbUser())
        {
            if (channel->getUserN(i) == user)
                i++;
            else
            {
                std::string from = user->getNick();
                sendMessage(channel->getUserN(i)->getClientFd(), from, input);
                i++;
            }
        }
    }
}
void    server::sendMessage(int clientFd, std::string from, std::string message)
{
    std::string userMsg;
    if (from.empty())
        userMsg = message;
    else
    {
        std::string sep = " : ";
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
            infoClient(currUser->getClientFd());
    }
    else
        std::cout << "curr User is null !!!!" << std::endl;
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


void    server::infoRequired(int clientFd)
{
    if (!getUserByFd(clientFd))
        sendMessage(clientFd, "IRC", "USER Info Required : USER and NICK (type HELP for usage)");
    else if (getUserByFd(clientFd)->getNick().empty())
        sendMessage(clientFd, "IRC", "USER Info required : NICK");
}
std::vector<std::string>    server::parsingIntoVector(std::string input)
{
    std::vector<std::string> command;
    input = trim_and_reduce_spaces(input);
    size_t start = 0;
    size_t end = input.find(" ");
    while (end != std::string::npos)
    {
        command.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(" ", start);
    }
    command.push_back(input.substr(start));
    return (command);
}
bool    server::manageUserInfo(int clientFd, std::string input)
{
    std::vector<std::string> command = parsingIntoVector(input);
    if (command.size() != 5 && command.size() != 2)  
        return (false);
    else if (command[0] == "USER")
    {
        if (manageUser(clientFd, command))
        {
            sendMessage(clientFd, "IRC", "USER register"); 
            return (true);
        }
        return (false);
    }
    else if (command[0] == "NICK")
    {
        if (manageNick(clientFd, command[1]))
        {
            sendMessage(clientFd, "IRC", "NICKNAME register"); 
            return (true);
        }
        return (false);
    }
    else
        return (false);
}
bool    server::manageUser(int clientFd, std::vector<std::string> command) 
{
    if (!isValidUsername(command[1]))
    {
        sendMessage(_clientFd, "IRC", "Not valid username");
        return (false);
    }
    else
    {
        user *newUser = new user(*this, clientFd, command);
        _nbClient++;
        newUser->setIdx(_nbClient);
        _userN[_nbClient] = newUser;
        return (true);
    }
}

user*  server::getUserByFd(int clientFd)
{
    for (int i = 1; i <= _nbClient; i++)
    {
        if (_userN[i]->getClientFd() == clientFd)
            return (_userN[i]);
    }
    return (NULL);
}
bool    server::isValidNickname(std::string nickname)
{
    for (std::vector<std::string>::iterator it = nicknameClient.begin(); it != nicknameClient.end(); it++)
    {
        if (*it == nickname)
            return (false);
    }
    if (!isalpha(nickname[0]) || nickname[0] == '@')
        return (false);
    return (true);
}
bool        server::manageNick(int clientFd, std::string nickname)
{
    if (!isValidNickname(nickname))
    {
        sendMessage(_clientFd, "IRC", "Not valid nickname, already use");
        return (false);
    }
    else
    {
        getUserByFd(clientFd)->setNickname(nickname);
        nicknameClient.push_back(nickname);       
        return (true);
    }
}
bool    server::isValidUsername(std::string username)
{
    if (!isalpha(username[0]))
        return (false);
    return (true);
}

void    server::infoClient(int clientFd)
{
    std::string info;
    std::string format = "$> ";
    std::string format2 = " ------IRC------[";
    std::string format3 = "]------>";
    user * curr = getUserByFd(clientFd);
    info = format + curr->getNick() + format2
    + curr->getCurrChannel() + format3;
    int msgSize = info.size();
    int byteS = send(clientFd, info.c_str(), msgSize, 0);
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
}

int     server::findFdClient(std::string user)
{
   int fd;
    for (int i = 1; i <= _nbClient; i++)
    {
        if (_userN[i]->getNick() == user)
            fd = _userN[i]->getClientFd();
    }
    return (fd);
}

void    server::delUserList(user * user)
{
    if (_loginClient.size() == 1)
        _loginClient.pop_back();
    else
    {
        for (std::vector<std::string>::iterator it = _loginClient.begin(); it != _loginClient.end(); it++)
        {
            if (*it == user->getUsername())
            {
                _loginClient.erase(it);
                break;
            }
        }
    }
}


void   server::checkChannel(std::string currChannel)
{
    if (_nbChannel > 0)
    {
        for (int i = 1; i <= _nbChannel; i++)
        {
            if (channelId[i]->getName() == currChannel)
            {
                if (channelId[i]->getNbUser() == 0)
                {
                    channelId[i]->~channel();
                    delete channelId[i];
                    _nbChannel--;
                }
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
void    server::receptInfo(std::string input, int clientFd)
{
    if (input == "HELP")
        sendMessage(clientFd, "IRC", "USER [username] [hostname(unused)] [servername(unused)] [realname]\n      NICK [nickname]");
    else
    {
        if (!manageUserInfo(clientFd, input))
            infoRequired(clientFd);
        else if (getUserByFd(clientFd) != NULL && getUserByFd(clientFd)->getNick().empty())
            infoRequired(clientFd);
        else
        {
            sendMessage(clientFd, "IRC", "Welcome !");
            printInfoNewUser(getUserByFd(clientFd));   
            infoClient(clientFd); 
        }
    }
}

void    server::readingClient(int clientFd)
{
    char buff[BUFSIZ] = {0};
    _bytesRead = recv(clientFd, buff, BUFSIZ - 1, 0);
    std::string input = buff;
    if (_bytesRead == -1)    
    {
        initError ex("recv");
        throw ex;
    }
    else if (_bytesRead == 0)
        closeFd(clientFd);
    else
    {
        buff[_bytesRead] = '\0';
        std::string input(buff);
        if (!getUserByFd(clientFd))
            receptInfo(input, clientFd);
        else if (getUserByFd(clientFd)->getNick().empty())
            receptInfo(input, clientFd);
        else
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
    struct pollfd clientPollFd;
    clientPollFd.fd = _clientFd;
    clientPollFd.events = POLLIN;
    _pollFds.push_back(clientPollFd);
    _clientFd = clientFd;
    infoRequired(clientFd);
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
                    std::cout << "New client connected, fd : " << _clientFd << std::endl;
                    handleClient(_clientFd);
                }
                else
                    readingClient(_pollFds[i].fd);
            }
        }
    }
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

void    server::printLoginList()
{
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
            << "Login client : "  << user->getUsername() << std::endl 
            << "Nickname client : " << user->getNick() << std::endl << std::endl;
}

void   server::printInfoUsers()
{
    for (int i = 1; i <= _nbClient; i++)
    {
        std::cout << "Socket client : " << _userN[i]->getClientFd() << std::endl
            << "Login client : "  << _userN[i]->getUsername() << std::endl 
            << "Nickname client : " << _userN[i]->getNick() << std::endl << std::endl;
    }
}
void    server::printCmd()
{
    for (std::vector<std::string>::iterator it = _command.begin(); it != _command.end(); it++)
        std::cout << *it << std::endl;
}
void    server::printChannelInfo()
{
    if (_nbChannel == 0)
        std::cout << "Actually no channel !" << std::endl;
    else
    {
        std::cout << "each channel info : " << std::endl;
        for (int i = 1; i <= _nbChannel; i++)
        {
            std::cout << "Channel name : " << channelId[i]->getName() << std::endl;
            std::cout << "Channel nb user : " << channelId[i]->getNbUser() << std::endl;
            for (int j = 1; j <= channelId[i]->getNbUser(); j++)
            {  
                std::cout << "-----------" << std::endl;
                std::string name = channelId[i]->getUserN(j)->getNick();
                int idx = channelId[i]->getIdxUserByName(name);
                std::cout << "name user in channel : " << name << std::endl;
                std::cout << "idx user in channel : " << idx << std::endl;
                std::cout << "-----------" << std::endl;
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

server::initError::initError(const std::string& error)
{
    std::strncpy(_error, "Error on : ", 99);
    std::strncat(_error, error.c_str(), 99 - std::strlen(_error));
    _error[99] = '\0';
}