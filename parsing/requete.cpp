/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requete.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:30 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/30 16:55:53 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "requete.hpp"

Requete::Requete(char *requete)
{
	std::cout << std::endl << "Before check " << std::endl;
	std::ofstream MyFile("my_input.txt");
	MyFile << requete;
	MyFile.close();
	_char_request = requete;
	std::stringstream ss(requete);
	_request = requete;
	std::cout << "String len = " << _request.size() << std::endl;
	std::cout << "Char len   = " << strlen(_char_request) << std::endl;
	_len = 0;
    ss >> this->_method;
    ss >> this->_url;
	ss >> this->_protocol;
	make_query();
	std::cout << "Method = " << _method << std::endl;
	if (_method == "GET")
	{
		std::cout << "---- Before GET " << std::endl;
		make_GET(ss);
	}
	else if (_method == "POST")
	{
		std::cout << "---- Before POST " << std::endl;
		make_POST(ss);

	}
	std::cout << "After check " << std::endl;
}

/* Check if request is good */
int Requete::check_tim()
{
	if (_method != "POST" && _method != "GET" && _method != "DELETE")
		return 405;
	if (_protocol != "HTTP/1.1")
		return 505;
	return -1;
}


/* check if content start */
void Requete::make_body(std::stringstream& ss, std::string token, size_t pos)
{
	std::string temp, save;
	(void)pos;
	while (!token.empty())
	{
		ss >> token;
		ss >> token;//Content-Disposition:
		ss >> token;//form-data;
		ss >> token;//name="file1";
		_name = token.substr(token.find("="));
		if (_name.back() == ';')
			_name.pop_back();//remove ;
		_name.pop_back();//remove " at end
		_name.erase(0, 2);//remove " at begin
		ss >> token;
		
		if (token.find("filename=") != std::string::npos)
		{
			_file_name = token.substr(token.find("="));
			_file_name.pop_back();//remove " at end
			_file_name.erase(0, 2);//remove " at begin
			ss >> token;
			if (token.find("Content-Type:") != std::string::npos)
			{
				ss >> token;
				_type = token;
				ss >> token;
			}
			size_t pos2 = _request.find(token) ;
			while (_char_request[pos2])
			{
				_body += _request[pos2];
				pos2++;
			}
			_body.erase(_body.size() - (_boundary.size() + 6));
		}
		break;
	}
}

/* Make post request */
void Requete::make_POST(std::stringstream& ss)
{
	std::string token, line, key;
	std::string buff;
	size_t pos = _request.find("\r\n\r\n");

	while (ss >> token)
	{
		// std::cout << "Token = " << token << std::endl;
		if (token.find("boundary=") != std::string::npos)
		{
			_boundary = token.substr(token.find("boundary=") + 9);
		}
		if (token == "Content-Length:")
		{
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();//remove space 
				_header.insert(std::pair<std::string, std::string>(key, line));
				line.clear();
			}
			key = token;
			ss >> token;
			_header.insert(std::pair<std::string, std::string>(key, token));
			_len = atoi(token.c_str());
			key.clear();
		}
		else if (_request.find(token) == pos - token.length())
		{
			pos += 4;
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();//remove space 
				_header.insert(std::pair<std::string, std::string>(key, line));
			}
			while (_char_request[pos])// || _request[pos + 1]
			{
				_full_body += _request[pos];
				pos++;
			}
			if (_boundary.empty())
				_body = _full_body;
			else
			{
				//make_body(ss, token, _request.find("\r\n\r\n") + 4);
			}
			break;
		}
		else if (token.back() == ':')
		{
			if (!key.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();
				_header.insert(std::pair<std::string, std::string>(key, line));
			}
			token.pop_back();
			key = token;
			line.clear();
		}
		else
		{
			if (!line.empty())
				line += " ";
			line += token;
		}
	}
	//print_all_data();
	//sleep(10);
	std::cout << "Request len = " << _request.size() << std::endl;
	std::cout << "BODY    len = " << _len << std::endl;
	std::cout << "_full_body len = " << _full_body.length() << std::endl;
	std::cout << "Full Body = " << std::endl << _full_body << std::endl;
	std::cout << "END Body" << std::endl;
	std::ofstream MyFile("my_parsing.txt");
	MyFile << _full_body;
	MyFile.close();
}

/* Make get request */
void Requete::make_GET(std::stringstream& ss)
{
	std::string token, line, key;

	while (ss >> token)
	{
		if (token.back() == ':')
		{
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();
				_header.insert(std::pair<std::string, std::string>(key, line));
			}
			token.pop_back();
			key = token;
			line.clear();
		}
		else
		{
			if (!line.empty())
				line += " ";
			line += token;
		}
	}
	if (!line.empty() && !key.empty())
	{
		if (line.back() == ' ')
			line.pop_back();
		_header.insert(std::pair<std::string, std::string>(key, line));
	}
	
}

/* Makes query for lxu-wu */
void Requete::make_query()
{
	size_t pos = _url.find("?");
	if (pos != std::string::npos)
		_query= _url.substr(pos + 1);
}


/* Print all data in requete */
void Requete::print_all_data()
{
	std::cout << "Method    = " << _method << std::endl;
	std::cout << "Url       = " << _url << std::endl;
	std::cout << "Protocol  = " << _protocol << std::endl;
	std::cout << "Boundary  = " << _boundary << std::endl;
	std::cout << "Name      = " << _name << std::endl;
	std::cout << "Filename  = " << _file_name << std::endl;
	std::cout << "Type      = " << _type << std::endl;
	std::cout << "Body      = " << _body << std::endl;
	//std::cout << "Full Body = " << _full_body << std::endl;
}