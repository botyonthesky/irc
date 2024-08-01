/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 17:21:53 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/08/01 17:57:15 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.hpp"

client::client()
{
    // std::cout << "Client default constructor" << std::endl;
}

client::~client()
{
    std::cout << "Client destructor" << std::endl;
}

void    client::initClient()
{
    _sa.sin_family = AF_INET; 
    _sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    _sa.sin_port = htons(PORT);
}

void    client::initSocket()
{
    _socketFd = socket(_sa.sin_family, SOCK_STREAM, 0);
    if (_socketFd == -1)
        socketFdError();
}

void    client::connectServer()
{
    _status = connect(_socketFd, (struct sockaddr *)&_sa, sizeof(_sa));
    if (_status != 0)
        connectError();
    std::cout << "Connected to localhost, PORT : " << PORT << std::endl;
}

void   client::handleMessage(void)
{
  
    if (_msg == "exit")
        throw exitSocket();
  
}

void    client::sendMessage()
{
    try
    {
        std::getline(std::cin, _msg);
        std::cout << _msg << std::endl;
        _msgLen = _msg.size();
        _bytesSent = send(_socketFd, _msg.c_str(), _msgLen, 0);
        if (_bytesSent == -1)
            sendError();
        else if (_bytesSent != _msgLen)
            std::cout << "Partial message send, only : " << _bytesSent << "bytes sent" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    client::receivedMessage()
{
    char buffer[BUFSIZ];
    _bytesRead = 1;
    while (_bytesRead >= 0) 
    {
        _bytesRead = recv(_socketFd, buffer, BUFSIZ, 0);
        if (_bytesRead == -1)
            recvError();
        else 
        {
            buffer[_bytesRead] = '\0';
            std::string buffS = buffer;
            std::string login = splitBuffer(buffS);
            std::string msg = splitBuffer2(buffS);
            std::cout << "Message from " << login << " : "  << msg << std::endl;
            break ;
        }
    }
}
void    client::run()
{
    initClient();
    initSocket();
    connectServer();
    
    fd_set readfds;
    int max_sd;
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(_socketFd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        max_sd = _socketFd;
        
        int retSelect = select(max_sd + 1, &readfds, NULL, NULL, NULL); 

        if (retSelect < 0)
        {
            std::cout << "Error on select" << std::endl;
        }
        if (FD_ISSET(_socketFd, &readfds))
            receivedMessage();
        if (FD_ISSET(STDIN_FILENO, &readfds))
            sendMessage();
    }
    std::cout << "We are closing socket fd : " << _socketFd << std::endl;
    close(_socketFd);
}

void    client::setMessage(std::string msg)
{
    _msg = msg;
}

const char* client::exitSocket::what() const throw()
{
    return ("We are closing socket");
}
const char* client::socketFdError::what() const throw()
{
    return ("Error on socket fd");
}

const char* client::sendError::what() const throw()
{
    return ("Error on send");
}

const char* client::recvError::what() const throw()
{
    return ("Error on recv");
}

const char* client::connectError::what() const throw()
{
    return ("error on connnect");
}