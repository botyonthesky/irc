/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:38:47 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/23 17:21:19 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

server::server()
{
    std::cout << "Server default constructor" << std::endl;
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
        socketFdError();
    std::cout << "Creation of server socket fd : " << _socketFd << std::endl;
}

void    server::initBind()
{
    _status = bind(_socketFd, (struct sockaddr *)&_sa, sizeof(_sa));
    std::cout << "Status of bind : " << _status << std::endl;
    if (_status != 0)
        bindError();
}

void    server::readingClient()
{
    _bytesRead = 1;
    char buff[BUFSIZ];
    while (_bytesRead >= 0)
    {
        std::cout << "We are gonna to read on client socket" << std::endl;
        _bytesRead = recv(_clientFd, buff, BUFSIZ, 0);
        if (_bytesRead == 0)
        {
            std::cout << "Client socket : " << _clientFd << " closed" << std::endl;
            break;
        }
        else if (_bytesRead == -1)    
            recvError();
        else
        {
            std::string message = "Got ya !";
            int msgSize = message.size();
            int bytesSend;
            buff[_bytesRead] = '\0';
            std::cout << "Message received from client socket : " << _clientFd
            << " -> " << buff << std::endl;
            bytesSend = send(_clientFd, message.c_str(), msgSize, 0);
            if (bytesSend == -1)
                sendError();
            else if (bytesSend == msgSize)
            {
            std::cout << "Full message received by client socket : " << _clientFd
                << " -> " << message.c_str() << std::endl;
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

void    server::run()
{
    try
    {
        initServer();
        initSocket();
        initBind();
        
        std::cout << "Connection socket to localhost, PORT : " << PORT << std::endl;
        std::cout << std::endl;
        
        std::cout << "Listen on port : " << PORT << std::endl;
        _status = listen(_socketFd, 20);
        
        std::cout << "Status of listen : " << _status << std::endl;
        if (_status != 0)
            listenError();
         
        _addrSize = sizeof(_clienAddr);
        
        std::cout << "Address size : " << _addrSize << std::endl;

        std::cout << "Waiting for client connection" << std::endl;
        
        while (true) 
        {
            _clientFd = accept(_socketFd, (struct sockaddr *)&_clienAddr, &_addrSize);
            std::cout << "Accept, client fd : " << _clientFd << std::endl;
            if (_clientFd == -1)
                clientFdError();
            std::cout << "Accept new connection on client fd : " << _clientFd << std::endl;
            handleClient(_clientFd);
        }
        std::cout << "Now closing client fd : " << _clientFd << std::endl;
        close(_clientFd);
        std::cout << "Now closing server fd : " << _socketFd << std::endl;
        close(_socketFd);    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    server::handleClient(int clientFd)
{
    _clientFd = clientFd;   
    readingClient();
}