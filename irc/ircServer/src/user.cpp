/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:03:04 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/05 15:17:3 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"
#include "../include/server.hpp"

user::user(server& srv, int clientFd, std::vector<std::string> command) : _server(srv), _clientFd(clientFd), _inChannel(false)
{
    std::cout << "User construct" << std::endl;
    try
    {
        if (!isValidName(command[1]))
            _server.sendMessage(clientFd, "IRC", "Not a valid username");
        _username = command[1];
        _name = command[4];
        _opChannel = false;
        _nickname = "";
        _currChannel = "No channel";
        _server.setLogin(_username);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

user::~user()
{
    _server.delUserList(this);
    std::cout << this->_username << " has quit the server" << std::endl;
}


bool    user::isValidName(std::string name)
{
    if (!isalpha(name[0]))
        return (false);
    return (true);
}

void    user::help()
{
    if (this->_opChannel)
    {
        std::string msg = "\nSpecific operator commands ->\n"
                        "KICK [nick name] [channel]         eject an user of the channel\n"
                        "INVITE [nick name] [channel]       invite an user to the channel\n"
                        "TOPIC [channel] [subject]          define subject as the topic of the channel";
        _server.sendMessage(_clientFd, "" , msg);
    }
    std::string msg = "\nBasic commands ->\n"
                        "/info                              display your information\n"
                        "/nick [nick name]                  change your nickname\n"
                        "/user [login]                      change your login\n"
                        "/join [channel]                    join channel\n"
                        "/leave                             leave current channel\n"
                        "/quit                              quit IRC\n"
                        "/who                               list of users in current channel\n"
                        "/msg [login] [msg]                 submit msg to login\n"
                        "/list                              list of channel\n"
                        "[msg]                              send msg to current channel\n";

    _server.sendMessage(this->_clientFd, "", msg);
}

void    user::info()
{
    std::string msg = "\nYour user name is : " + _username + ", your nick name is : "+ _nickname;
    std::string channel;
    if (!_inChannel)
        channel = "\nYou re not in any channel right now !\n";
    else
        channel = "\nYou re in the channel : " + _currChannel;
    _server.sendMessage(this->_clientFd, "" , msg + channel);
}

void    user::speCommandOp()
{
    if (this->_opChannel)
    {

    }
    else
    {
        _server.sendMessage(_clientFd, "IRC", "You're not operator inm this channel");
    }
}

void    user::leave()
{
   if (_currChannel == "No channel")
    {
        std::string msg = "\nYou re actually in any channel\n";
        _server.sendMessage(_clientFd, "", msg);
    }
    else
    {
        channel * curr = getChannelByName(_currChannel);
        std::cout << "curr chan " << curr->getName() << std::endl;
        int idx = curr->getIdxUserByName(this->_username);
        curr->delUserN(idx);
        _opChannel = false;
        _inChannel = false;
        std::string msg = "You left the channel : " + _currChannel;
        _server.sendMessage(_clientFd, "" , msg);
        std::cout << _nickname << "  left channel : " << _currChannel << std::endl;
        _server.checkChannel(_currChannel);
        _currChannel = "No channel";
    }
}

void    user::nick()
{
    try
    {
        if (!isValidName(_server.getCommand()[1]))
        {
            _server.sendMessage(_clientFd, "IRC", "Not valid nickname");
            throw NotValidNickName();
        }
        std::string oldNick = _nickname;
        std::string newNick = _server.getCommand()[1];
        std::string msg = "Your nickname was : " + oldNick + " its now : " + newNick;
        _nickname = newNick;
        _server.sendMessage(this->_clientFd, "", msg);
        std::cout << _username << " has changed his nickname to : " << _nickname << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void   user::userName()
{
    try
    {
        if (!isValidName(_server.getCommand()[1]))
        {
            _server.sendMessage(_clientFd, "IRC", "Not valid username");
            throw NotValidUserName();
        }
        std::string newName = _server.getCommand()[1];
        std::string oldName = _username;
        _server.updateLoginList(oldName, newName);
        _username = newName;
        std::string msg = "You username was : " + oldName + ", it's now : " + newName;
        _server.sendMessage(this->_clientFd, "", msg);
        std::cout << oldName << " has changed his username to : " << newName << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

int    user::checkChannel()
{
    
    for (int i = 1; i <= _server.getNbChannel(); i++)
    {
        if (_server.getCommand()[1] == _server.channelId[i]->getName())
            return (_server.channelId[i]->getIdx());
    }
    return (-1);
}


void    user::registerChannel(std::string name, channel * channel)
{

    for (int i = 1; i <= _server.getNbChannel(); i++)
    {
        if (_server.channelId[i]->getName() == name)
            channelUser[i] = channel;
    }
}
void    user::createNewChannel(std::string name)
{
    channel *newChannel = new channel(this, name);
    _inChannel = true;
    std::cout << _nickname << " has created and join the channel : " << name << std::endl;
    _currChannel = name;
    _opChannel = true;
    _server.setNbChannel(1);
    newChannel->setIdx(_server.getNbChannel());
    _server.channelId[newChannel->getIdx()] = newChannel;
    std::string msg = "Channel created and joined : " + _currChannel;
    _server.sendMessage(_clientFd, "" , msg);
    registerChannel(name, newChannel);
}

void    user::joinChannel(std::string name)
{
    _inChannel = true;
    std::cout << _nickname << " has join the channel : " << _server.getCommand()[1] << std::endl;
    _currChannel = _server.getCommand()[1];
    std::string msg = "Channel joined : " + _currChannel;
    _server.sendMessage(_clientFd, "", msg);
    _server.channelId[checkChannel()]->setNbUser(1);
    registerChannel(name, _server.channelId[checkChannel()]);
    channel * curr = getChannelByName(name);
    int idx = _server.channelId[checkChannel()]->getNbUser();
    curr->setUserN(this, idx);
}
void    user::join()
{
    std::string name = _server.getCommand()[1];
    try
    {
        if (!isValidChannelName(name))
        {
            _server.sendMessage(_clientFd, "IRC" , "Not valid Channel name\nUsage : ('#' | '&') <chstring>");
            throw NotValidChannelName();
        }
        if (checkChannel() == -1)
            createNewChannel(name);
        else
            joinChannel(name);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
    
bool    user::isValidChannelName(std::string name)
{
    if (name[0] == '&' || name[0] == '#')
        return (true);
    return (false);
}

void    user::msg()
{
    if (!checkUserList())
    {
        _server.sendMessage(_clientFd, "", "The username is not valid !");
        // throw NotValidUserName();
    }
    else
    {
        int fdToSend = _server.findFdClient(_server.getCommand()[1]);
        _server.sendMessage(fdToSend, this->getNick(), _server.getCommand()[2]);
    }
}

void    user::who()
{
    std::string msg;
    if (!_inChannel)
        msg = "You re not in any channel right now !";
    else
    {
        channel *curr = getChannelByName(_currChannel);
        if (curr == NULL)
        {
            std::cerr << "Error on /who" << std::endl;
            return ;
        }
        int nb = curr->getNbUser();
        msg = "\nYou are actually in the channel : " + _currChannel + "\nThere is " + toStr(nb) + " client(s) in this channel -> \n";
        if (curr->getNbUser() == 1)
        {
            msg += "User : " + this->_nickname;
        }
        else
        {
            for (int i = 1; i <= curr->getNbUser(); i++)
            {
                std::string nick = curr->getUserN(i)->getNick();
                msg += "User : " + nick + "\n";
            }
        }
    }
    _server.sendMessage(_clientFd, "", msg);
}

void    user::list()
{
    std::string msg;
    if (_server.getNbChannel() == 0)
        msg = "There is actually any channel in this server";
    else
    {      
        msg = "There is actually : " + toStr(_server.getNbChannel()) + " channel(s) =>\n";
        for (int i = 1; i <= _server.getNbChannel(); i++)
        {
            std::string name = _server.channelId[i]->getName();
            std::string nbs = toStr(_server.channelId[i]->getNbUser());
            msg += name + " [ " + nbs + " users ]" + "\n";
        } 
    }
    _server.sendMessage(_clientFd, "", msg);
}
bool    user::checkUserList()
{
    std::vector<std::string> tmp = _server.getLogin();
    if (tmp.size() == 1)
        return (false);
    else
    {
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
        {
            if (_server.getCommand()[1] == *it)
                return (true);
        }
    }   
    return (false);
    
}
int             user::getClientFd(void)
{
    return (_clientFd);    
}
int            user::getIdx(void)
{
    return (_idx);
}

void        user::setIdx(int idx)
{
    _idx = idx;
}

void        user::decrementIdx()
{
    _idx--;
}

std::string     user::getName(void)
{
    return (_username);
}

std::string     user::getNick(void)
{
    return (_nickname);
}
std::string     user::getCurrChannel(void)
{
    return (_currChannel);
}
bool            user::getInChannel()
{
    return (_inChannel);
}

bool            user::getOpChannel()
{
    return (_opChannel);
}

void        user::setOpchannel(bool op)
{
    _opChannel = op;
}
channel*    user::getChannelByIdx(int idx)
{
    return (channelUser[idx]);
}

channel*     user::getChannelByName(std::string name)
{
    for (int i = 1; i <= _server.getNbChannel(); i++)
    {
        if (_server.channelId[i]->getName() == name)
            return (channelUser[i]);
    }
    return (NULL);
}
void    user::setNickname(std::string nickname)
{
    _nickname = nickname;
}
const char*     user::NotValidUserName::what() const throw()
{
    return ("The username is not valid !");
}

const char*     user::NotValidNickName::what() const throw()
{
    return ("The nickname is not valid !");
}

const char* user::NotValidChannelName::what() const throw()
{
    return ("The channel name is not valid\nUsage : ('#' | '&') <chstring>");
}

