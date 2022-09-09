/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   servers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/31 14:41:15 by tmartial          #+#    #+#             */
/*   Updated: 2022/09/06 14:00:12 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
/* check locations are not empty */
bool Servers::check_locations()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i]->getDir().empty())
			return false;
		if (_locations[i]->getMethod().empty())
			return false;
		if (_locations[i]->getRoot().empty())
			return false;
		if (_locations[i]->getIndex().empty())
			return false;
		if (_locations[i]->getListing().empty())
			return false;
		if (_locations[i]->getRedir().empty())
			return false;
	}
	return true;
}
/* check if all locations have index*/
bool Servers::check_index()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i]->getIndex().empty())
			return false;
		else if (_locations[i]->getIndex().find(".html") == std::string::npos)
			return false;
	}
	return true;
}

/* check if listing is off or on */
bool Servers::check_listing()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i]->getListing() != "on" && _locations[i]->getListing() != "off")
			return false;
	}
	return true;
}

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

/* Check if error pages are good with nums and .html */
bool Servers::check_error_page()
{
	std::map<std::string, std::string>::iterator it = _error.begin();
	std::map<std::string, std::string>::iterator it_end = _error.end();
	while (it != it_end)
	{
		// std::cout << "it = " << it->first << std::endl;
		if (!my_atoi(it->first) || (it->second).find(".html") == std::string::npos)
			return false;
		it++;
	}
	return true;
}

/* Check for root */
bool Servers::check_root()
{
	if (_root.length() <= 2)
	{
		if (_root == "./")
			return true;
		return false;
	}
	if (_root[_root.length() - 1] != '/')
		_root += "/";
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i]->getRoot().length() <= 2)
		{
			if (_locations[i]->getRoot() == "./")
				continue;
			return false;
		}
		if (_locations[i]->getRoot()[_locations[i]->getRoot().length() - 1] != '/')
			_locations[i]->setRoot(_locations[i]->getRoot() + "/");
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
	{
		if (!_locations[pos]->getRoot().empty())
			throw DirTwice();
		_locations[pos]->setRoot(last);
	}
	else if (word == "index")
	{
		if (!_locations[pos]->getIndex().empty())
			throw DirTwice();
		_locations[pos]->setIndex(last);
	}
	else if (word == "allowed_methods")
		_locations[pos]->setMethod(last);
	else if (word == "dir_listing")
	{
		if (!_locations[pos]->getListing().empty())
			throw DirTwice();
		_locations[pos]->setListing(last);
	}
	else if (word == "redir")
	{
		if (!_locations[pos]->getRedir().empty())
			throw DirTwice();
		_locations[pos]->setRedir(last);
	}
	
}