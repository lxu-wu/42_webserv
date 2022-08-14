/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/09 15:53:02 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/13 17:44:43 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <set>
# include <cctype>
# include <string>
using std::cout;
using std::endl;
using std::cerr;

class Location;
class Server;
class Conf;

/* CHANGE ALL SENTENCE BY LINE */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!! */


/* PARSE_CONF */
void parsing(int argc, char **argv, Conf &data);
void parse_basic(int argc, char **argv);
int count_words(std::string sentence);
std::string ft_first_word(std::string line);


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

		std::vector<std::string> get_file() const;

		/* Functions */
		void read_file(std::string name);
		void check_conf();
		void check_brackets();
		bool is_directive(std::string sentence);
	
	private:
		std::vector<Server*> _servers;
		std::vector<std::string> _file;
		std::vector<std::string> _directives;
		
};



#endif