/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: botyonthesky <botyonthesky@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:45:13 by botyonthesk       #+#    #+#             */
/*   Updated: 2024/07/26 11:11:45 by botyonthesk      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    std::cout << "[ SERVER ]" << std::endl;
    server ircserv;

        ircserv.run();       
    
   
    
   

     
    
    return (0);
}