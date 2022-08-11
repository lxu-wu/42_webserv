/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/09 15:53:02 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/11 18:10:38 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <vector>
using std::cout;
using std::endl;
using std::cerr;

/* PARSE_CONF */
void parsing(int argc, char **argv);


class Location
{
	public:
		Location();
		~Location();
		
	private:
		std::string _dir;
		std::string _upload;
		std::string _method;
		std::string _root;
		std::string _index;
		
};

class Server
{
	public:
		Server();
		~Server();

	private:
		std::string _listen;
		std::string _name;
		std::string _method;
		std::string _root;
		std::string _index;
		std::string _body_size;

		std::vector<Location*> _locations;

};

class Conf
{
	public:
		Conf();
		~Conf();
	
	private:
		std::vector<Server*> _servers;
		
};



#endif