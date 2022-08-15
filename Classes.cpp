/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Classes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 14:48:09 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/15 15:35:23 by tmartial         ###   ########.fr       */
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
	_directives.push_back("}");
	_directives.push_back("{");
}

Conf::~Conf()
{
	
}

std::vector<std::string> Conf::get_file() const
{
	return (this->_file);
}

/* --- FUNCTIONS --- */
void Conf::check_conf()
{
	
}

/* Check if all lines are directive */
void Conf::check_directive()
{
	std::size_t len = _file.size();

	for (size_t i = 0; i < len; i++)
	{
		if (!this->is_directive(_file[i]))
		{
			cerr << "Error: Conf file is not in good format" << endl;
			exit (1);
		}
	}
	
}

/* Check if word is a directive */
bool Conf::is_directive(std::string sentence)
{
	std::size_t count = count_words(sentence), len = _directives.size();
	std::string word = ft_first_word(sentence);

	for (size_t i = 0; i < len; i++)
	{
		if (word == _directives[i])
		{
			if (count == 1 && word != "}" && word != "{" && word != "server")
				return false;
			else if (count >= 3 && word != "allowed_methods" && word != "location" && word != "error_page")
				return false;
			return true;
		}
	}
	return false;
}

/* Check if directives are written in servers */
/* remonte le vecteur jusque trouve server sinon faux */

/* Check if brackets are closed */
void Conf::check_brackets()
{
	std::size_t left = 0, right = 0, len = _file.size();

	for (size_t i = 0; i <= len; i++)
	{
		if (_file[i].find("{") != std::string::npos)
			left++;
		if (_file[i].find("}") != std::string::npos)
			right++;
	}
	if (left != right)
	{
		cerr << "Error: Not good numbers of brackets" << endl;
		exit (1);
	}
}

/* Fill data will all read lines 
   Insert in _file all lines except empty line */
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

