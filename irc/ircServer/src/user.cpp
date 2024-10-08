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

user::user(server& srv, int clientFd, std::vector<std::string> command) : _server(srv), _clientFd(clientFd), _inChannel(false)
{
    std::cout << "User construct" << std::endl;
    try
    {
        if (!isValidUsername(command[1]))
            _server.sendMessage(clientFd, "IRC", "Not a valid username");
        _username = command[1];
        _hostname = command[3];
        _realName = command[4];
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

void    user::help()
{
    if (this->_nickname[0] == '@')
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
                        "/user [user name]                  change your login\n"
                        "/join [channel]                    join channel\n"
                        "/leave                             leave current channel\n"
                        "/quit                              quit IRC\n"
                        "/who                               list of users in current channel\n"
                        "/msg [nick name] [msg]             submit msg to login\n"
                        "/list                              list of channel\n"
                        "[msg]                              send msg to current channel\n";

    _server.sendMessage(this->_clientFd, "", msg);
}

void    user::info()
{
    std::string msg = "\nYour username is : " + _username + ".\nYour nickname is : "+ _nickname;
    std::string channel;
    if (!_inChannel)
        channel = "\nYou re not in any channel right now !\n";
    else
        channel = "\nYou re in the channel : " + _currChannel;
    _server.sendMessage(this->_clientFd, "" , msg + channel);
}

void    user::nick()
{
    if (_server.getCommand().size() != 2)
    {
        _server.sendMessage(_clientFd, "", "Invalid Format. Usage : /nick [nick name]");
        return ;
    }
    try
    {
        if (!isValidNickname(_server.getCommand()[1]))
        {
            _server.sendMessage(_clientFd, "IRC", "Not valid nickname");
            throw NotValidNickName();
        }
        std::string oldNick = _nickname;
        std::string newNick = _server.getCommand()[1];
        _server.updateNicknameList(oldNick, newNick);
        std::string msg = "Your nickname was : " + oldNick + " its now : " + newNick;
        if (_nickname[0] == '@')
            _nickname = "@" + newNick;
        else
            _nickname = newNick;
        _server.sendMessage(this->_clientFd, "", msg);
        std::cout << oldNick << " has changed his nickname to : " << _nickname << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void   user::userName()
{
    if (_server.getCommand().size() != 2)
    {
        _server.sendMessage(_clientFd, "", "Invalid Format. Usage : /user [user name]");
        return ;
    }
    try
    {
        if (!isValidUsername(_server.getCommand()[1]))
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

void    user::join()
{
    if (_server.getCommand().size() != 2)
    {
        _server.sendMessage(_clientFd, "", "Invalid Format. Usage : /join [channel name]");
        return ;
    }
    std::string name = _server.getCommand()[1];
    try
    {
        if (!isValidChannelName(name))
        {
            _server.sendMessage(_clientFd, "IRC" , "Not valid Channel name\nUsage : ('#' | '&') <chstring>");
            throw NotValidChannelName();
        }
        if (_currChannel != "No channel")
            leave();
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
        int idx = curr->getIdxUserByNickname(this->_nickname);
        curr->delUserN(idx);
        if (_nickname[0] == '@')
            setNickname(_nickname.erase(0, 1));
        _inChannel = false;
        std::string msg = "You left the channel : " + _currChannel;
        _server.sendMessage(_clientFd, "" , msg);
        std::cout << _nickname << "  left channel : " << _currChannel << std::endl;
        _server.checkChannel(_currChannel);
        _currChannel = "No channel";
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
            msg += "User : " + this->_nickname;
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

void    user::msg()
{
    if (_server.getCommand().size() != 3)
    {
        _server.sendMessage(_clientFd, "", "Invalid Format. Usage : /msg [nickname] [msg]");
        return ;
    }
    if (!checkNicknameList(_server.getCommand()[1]))
        _server.sendMessage(_clientFd, "", "The username is not valid !");
    else
    {
        std::cout << "else : " << std::endl; 
        int fdToSend = _server.getFdClientByNick(_server.getCommand()[1]);
        std::cout << "fd to send  : " << fdToSend << std::endl;
        _server.sendMessage(fdToSend, this->getNick(), _server.getCommand()[2]);
    }
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

void    user::kick()
{
    if (!checkUser())
        return ;
    if (!checkKickInfo())
        return ;
    else
    {
        std::string nameK = _server.getCommand()[1];
        user * toKick = findUserChannelByName(nameK);
        channel * curr = getChannelByName(_currChannel);
        int idx = curr->getIdxUserByNickname(nameK);
        curr->delUserN(idx);
        toKick->_currChannel = "No channel";
        toKick->_inChannel = false;
        std::string msg = "You have been ejected form the channel : " + _currChannel;
        _server.sendMessage(toKick->getClientFd(), "" , msg);
        std::cout << _server.getCommand()[1] << " left channel : " << _currChannel << std::endl;
        _server.checkChannel(_currChannel);
        _server.infoClient(toKick->getClientFd());
    }
    return ;
}

void    user::invite()
{   
    if (!isOperatorOfChannel(_server.getCommand()[2]))
        return ;
    else
    {
        std::string nickTo = _server.getCommand()[1];
        std::string channelTo = _server.getCommand()[2];
        user * toSend = _server.getUserByNickname(nickTo);
        if (toSend == NULL)
            std::cout << "error on invite, tosend is null" << std::endl;
        else
        {
            std::string msgFrom;
            msgFrom = ":" + _nickname + " INVITE";
            std::string msgTo;
            msgTo = nickTo + " to channel " + channelTo;
            _server.sendMessage(toSend->getClientFd(), msgFrom, msgTo);
        }
    }
}

void    user::createNewChannel(std::string name)
{
    channel *newChannel = new channel(this, name);
    _inChannel = true;
    std::cout << _nickname << " has created and join the channel : " << name << std::endl;
    _currChannel = name;
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
    if (curr->getNameOperator() == _nickname)
        _nickname = "@" + _nickname;

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
bool    user::checkUser()
{
    if (_server.getCommand().size() != 3)
    {
        _server.sendMessage(_clientFd, "IRC", "Not valid format. Usage : KICK [nick name] [channel]");
        return (false);
    }
    if (_inChannel == false)
    {
        _server.sendMessage(_clientFd, "", "You re actually in any channel !");
        return (false);
    }
    if (_currChannel != _server.getCommand()[2])
    {
        _server.sendMessage(_clientFd, "IRC", "You re not in the specify channel");
        return (false);
    }
    if (_nickname[0] != '@')
    {
        _server.sendMessage(_clientFd, "IRC", "You re not operator of this channel");
        return (false);
    }
    return (true);
}
bool    user::checkUserChannelList()
{
    channel * curr = getChannelByName(_currChannel);
    for (int i = 1; i <= curr->getNbUser(); i++)
    {
        if (curr->getUserN(i)->getNick() == _server.getCommand()[1])
            return (true);
    }
    return (false);
}


bool    user::checkNicknameList(std::string nickname)
{
    std::vector<std::string> tmp = _server.nicknameClient;
    if (tmp.size() == 1)
        return (false);
    else
    {
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
        {
            if (nickname == *it)
            {
                std::cout << "find nick name !" << std::endl;
                return (true);
            }
        }
    }   
    return (false);
}
bool    user::checkKickInfo()
{
    if (!checkUserChannelList())
    {
        _server.sendMessage(_clientFd, "IRC", "Nickname not found in this channel !");
        return (false);
    }
    if (_currChannel != _server.getCommand()[2])
    {
        _server.sendMessage(_clientFd, "IRC", "You re not in the specify channel");
        return (false);
    }
    return (true);
}

void    user::registerChannel(std::string name, channel * channel)
{
    for (int i = 1; i <= _server.getNbChannel(); i++)
    {
        if (_server.channelId[i]->getName() == name)
            channelUser[i] = channel;
    }
}   

user*  user::findUserChannelByName(std::string name)
{
    channel * curr = getChannelByName(_currChannel);
    for (int i = 1; i <= curr->getNbUser(); i++)
    {
        if (curr->getUserN(i)->getNick() == name)
            return (curr->getUserN(i));
    }
    return (NULL);
}

bool    user::isOperatorOfChannel(std::string channel)
{
    if (_server.getCommand().size() != 3)
    {
        _server.sendMessage(_clientFd, "IRC", "Invalid format. Usage : INVITE [nickname] [channel]");
        return (false);
    }
    if (channel != _currChannel)
    {
        _server.sendMessage(_clientFd, "IRC", "You re not in this channel");
        return (false);
    }
    if (_nickname[0] != '@')
    {
        _server.sendMessage(_clientFd, "IRC", "You re not operator of this channel");
        return (false);
    }
    if(!checkNicknameList(_server.getCommand()[1]))
    {
        _server.sendMessage(_clientFd, "IRC", "Not valid nickname");
        return (false);
    }
    return (true);
}

bool    user::isValidChannelName(std::string name)
{
    if (name[0] == '&' || name[0] == '#')
    {
        for (size_t i = 0; i < name.size(); i++)
            if(!isprint(name[i]))
                return (false);
        return (true);
    }
    return (false);
}

bool    user::isValidUsername(std::string username)
{
    if (!isalpha(username[0]))
        return (false);
    return (true);
}

bool    user::isValidNickname(std::string nickname)
{
    for (std::vector<std::string>::iterator it = _server.nicknameClient.begin(); it != _server.nicknameClient.end(); it++)
    {
        if (*it == nickname)
            return (false);
    }
    if (!isalpha(nickname[0]) || nickname[0] == '@')
        return (false);
    return (true);
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

std::string     user::getUsername(void)
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

channel*    user::getChannelByIdx(int idx)
{
    return (channelUser[idx]);
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

