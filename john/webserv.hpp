/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecorte42 <jdecorte42@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/09 15:53:02 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/20 11:42:39 by jdecorte42       ###   ########.fr       */
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

/* john */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <istream>

using std::cout;
using std::endl;
using std::cerr;

class Location;
class Servers;
class Conf;

/* CHANGE ALL SENTENCE BY LINE */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* REQUEST
GET / HTTP/1.1
Host: localhost:8080
Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: "Opera";v="89", "Chromium";v="103", "_Not:A-Brand";v="24"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "macOS"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.5060.134 Safari/537.36 OPR/89.0.4447.83
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.9
*/


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

class Servers
{
	public:
		Servers();
		~Servers();

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
		std::vector<Servers*> getServers() const { return _servers; }


		/* Functions */
		void read_file(std::string name);
		void check_conf();
		void check_brackets();
		bool is_directive(std::string sentence);
		void check_directive();
	
		std::vector<Servers*> _servers;
		std::vector<std::string> _file;
		std::vector<std::string> _directives;
		
};



#endif