/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Classes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 14:48:09 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/19 16:45:24 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/* ---------------------------------------------------- */
/*                                                      */
/*                      SERVER                          */
/*                                                      */
/* ---------------------------------------------------- */

Servers::Servers()
{
	
}

Servers::~Servers()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		delete _locations[i];
	}
	_locations.clear();
}

/* Functions */

/* Check methods are correct in one server and it locations */
bool Servers::check_method()
{
	for (size_t j = 0; j < _method.size(); j++)
	{
		if ( _method[j] != "POST" &&  _method[j] != "GET" &&  _method[j] != "DELETE" )
			return false;
	}
	for (size_t i = 0; i < _locations.size(); i++)
	{
		for (size_t j = 0; j < _locations[i]->getMethod().size(); j++)
		{
			if (_locations[i]->getMethod()[j] != "POST" && _locations[i]->getMethod()[j] != "GET"
			&& _locations[i]->getMethod()[j] != "DELETE" )
				return false;
		}
	}
	return true;
}

/* Stock location */ 
void Servers::stock_location(std::string line, int pos)
{
	std::string word = ft_first_word(line);
	std::string last = ft_last_word(line);
	
	if (word == "location")
		_locations[pos]->setDir(last);
	else if (word == "root")
		_locations[pos]->setRoot(last);
	else if (word == "index")
		_locations[pos]->setIndex(last);
	else if (word == "allowed_methods")
		_locations[pos]->setMethod(last);
	
}

/* ---------------------------------------------------- */
/*                                                      */
/*                        CONF                          */
/*                                                      */
/* ---------------------------------------------------- */

Conf::Conf()
{	
	_directives.push_back("server");
	_directives.push_back("listen");
	_directives.push_back("server_name");
	_directives.push_back("allowed_methods");
	_directives.push_back("root");
	_directives.push_back("error_page");
	_directives.push_back("index");
	_directives.push_back("client_max_body_size");
	_directives.push_back("location");
}

Conf::~Conf()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		delete _servers[i];
	}
	_servers.clear();
}


/* --- FUNCTIONS --- */

/* Check all data is correct */
void Conf::check_data()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i]->getName().empty() || _servers[i]->getListen().empty() || _servers[i]->getRoot().empty() 
			|| _servers[i]->getIndex().empty() || _servers[i]->getMethod().empty()
			|| _servers[i]->getError().empty() || _servers[i]->getBody().empty())
			throw DirMissing();
		else if (!my_atoi(_servers[i]->getListen()) || !my_atoi(_servers[i]->getBody()))
			throw NotINT();
		else if (!_servers[i]->check_method())
			throw MethWrong();
		
	}
	
}

/* Print all data in conf */
void Conf::print_all_data()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		cout << "--- server "<< i << ":" << endl;
		cout << "name = " << _servers[i]->getName() << endl;
		cout << "listen = " << _servers[i]->getListen() << endl;
		cout << "root = " << _servers[i]->getRoot() << endl;
		cout << "index = " << _servers[i]->getIndex() << endl;
		cout << "body = " << _servers[i]->getBody() << endl;
		cout << "methods = ";
		for (size_t len = 0; len < _servers[i]->getMethod().size(); len++)
			cout << _servers[i]->getMethod()[len] << " ";
		cout << endl;
		cout << "error pages:" << endl;
		std::map<std::string, std::string> copy = _servers[i]->getError();
		std::map<std::string, std::string>::iterator it = copy.begin();
		for (size_t len = 0; len < _servers[i]->getError().size(); len++)
		{
		 	cout << "error " << it->first << " = " << it->second << endl;
			it++;
		}

		for (size_t x = 0; x < _servers[i]->getLocation().size(); x++)
		{
			cout << "- location "<< x << ":" << endl;
			cout << "dir = " << _servers[i]->getLocation()[x]->getDir() << endl;
			cout << "root = " << _servers[i]->getLocation()[x]->getRoot() << endl;
			cout << "index = " << _servers[i]->getLocation()[x]->getIndex() << endl;
			cout << "methods = ";
			for (size_t len = 0; len < _servers[i]->getLocation()[x]->getMethod().size(); len++)
				cout << _servers[i]->getLocation()[x]->getMethod()[len] << " ";
			cout << endl;
		}
	}
}

/* Vector with pos of directive if location or server */
void Conf::init_file_pos()
{
	//0 = server; 1 = location;
	size_t len =_file.size(), pos = 0;
	std::string word;

	for (size_t i = 0; i < len; i++)
	{
		word = ft_first_word(_file[i]);
		if (word == "server")
			pos = 0;
		else if (word == "location")
			pos = 1;
		_file_pos.push_back(pos);
	}
}

/* Check if all lines are directive */
void Conf::check_directive()
{
	std::size_t len = _file.size();

	for (size_t i = 0; i < len; i++)
		this->is_directive(_file[i], i);
}

/* Stock all the datas */
void Conf::stock_data()
{
	std::size_t len = _file.size();
	int nb_server = -1, nb_locations = -1;
	
	for (size_t i = 0; i < len; i++)
	{
		if (_file_pos[i] == 0)
		{
			if (ft_first_word(_file[i]) == "server")
			{
				setServers();//New server
				nb_server++;
				nb_locations = -1;
			}
			else
			{
				stock_server(_file[i], _servers[nb_server]);
			}
		}
		else
		{
			if (ft_first_word(_file[i]) == "location")
			{
				_servers[nb_server]->setLocation();//New locations
				nb_locations++;
			}
			_servers[nb_server]->stock_location(_file[i], nb_locations);
		}
	}
}

/* Stock directive in server */
void Conf::stock_server(std::string line, Servers* server)
{
	std::size_t count = count_words(line);
	std::string word = ft_first_word(line), last;
	
	if (count == 2)
	{
		last = ft_last_word(line);
		if (word == "listen")
			server->setListen(last);
		else if (word == "server_name")
			server->setName(last);
		else if (word == "root")
			server->setRoot(last);
		else if (word == "index")
			server->setIndex(last);
		else if (word == "client_max_body_size")
			server->setBody(last);
		else if (word == "allowed_methods")
			server->setMethod(last);
	}
	else if (count >= 3)
	{
		std::stringstream ss(line);
		std::string token;
		last = ft_last_word(line);

		while (ss >> token)
		{
			if (token != last && token != word)
    			server->setError(token, last);
		}
	}
	
}

/* Check if word is a directive */
void Conf::is_directive(std::string line, int pos)
{
	std::size_t count = count_words(line), len = _directives.size();
	std::string word = ft_first_word(line);

	for (size_t i = 0; i < len; i++)
	{
		if (word == _directives[i])
		{
			if (count == 1 && word != "server")
				throw MissingArgv();
			else if (count >= 3 && word != "error_page")
				throw TooMuchArgv();
			else if (count == 2 && word == "error_page")
				throw MissingArgv();
			else if (_file_pos[pos] == 1 && (word == "listen" || word == "client_max_body_size" || word == "server_name")) //0 == server, 1 == location
				throw DirWrongPlace();
			return ;
		}
	}
	throw DirWrong();
}

/* Fill data will all read lines Insert in _file all lines except empty line */
void Conf::read_file(std::string name)
{
	std::ifstream file(name);
	std::string output;

	while (std::getline(file, output))
	{
		std::size_t len = output.length();

		for (std::size_t i = 0; i < len; i++)
		{
			if (!isspace(output[i]))
			{
				_file.push_back(output);
				break;
			}
		}
	}
}


/* ---------------------------------------------------- */
/*                                                      */
/*                      LOCATION                        */
/*                                                      */
/* ---------------------------------------------------- */

Location::Location()
{
	
}

Location::~Location()
{
	
}

/* Functions */
