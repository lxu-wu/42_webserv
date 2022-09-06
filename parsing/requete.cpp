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

Requete::Requete(std::string requete)
{
	std::cout << std::endl << "Before check " << std::endl;
	std::stringstream ss(requete);
	_request = requete;
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
void Requete::make_body(std::stringstream& ss, std::string token)
{
	std::string temp, save;

	while (!token.empty())
	{
		if (!(ss >> token))//Content-Disposition:
			break;
		ss >> token;//form-data;
		ss >> token;//name="file1";
		_name = token.substr(token.find("\""));
		_name.pop_back();//;
		_name.pop_back();//"
		ss >> token;
		
		if (token.find("filename=") == std::string::npos)
		{
			while (!token.empty() && token.find(_boundary) == std::string::npos)
			{
				temp += token;
				temp += " ";
				save = token;
				ss >> token;
				if (save == token)
					break;
			}
			_text.insert(std::pair<std::string, std::string>(_name, temp));
			_body += temp;
			save.clear();
			temp.clear();
		}
		else
		{
			_file_name = token.substr(token.find('"'));
			_file_name.pop_back();//"
			ss >> token;//Content-Type:
			ss >> token;
			_type = token;
			ss >> token;
			while (!token.empty() && token.find(_boundary) == std::string::npos)
			{
				temp += token;
				temp += " ";
				save = token;
				ss >> token;
				if (save == token)
					break;
			}
			_body += temp;
			save.clear();
			temp.clear();
		}
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
			//make_body(ss, token);
			while (_request[pos])// || _request[pos + 1]
			{
				_full_body += _request[pos];
				pos++;
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
	std::cout << "Request len = " << _request.size() << std::endl;
	std::cout << "BODY    len = " << _len << std::endl;
	std::cout << "_full_body len = " << _full_body.length() << std::endl;
	std::cout << "Full Body = " << std::endl << _full_body << std::endl;
	std::cout << "END Body" << std::endl;
	make_full_body();
}

/* Make get request */
void Requete::make_GET(std::stringstream& ss)
{
	std::string token, line, key;

	while (ss >> token)
	{
		if (token.back() == ':')
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

void Requete::make_full_body()
{
	//_full_body = _body;
	//std::cout << "request len = " << _request.length() << std::endl;
	//std::cout << "len = " << _len << std::endl;
	//_full_body = _request.substr(_request.length() - _len);
}

/* Print all data in requete */
void Requete::print_all_data()
{
	std::cout << "Method = " << _method << std::endl;
	std::cout << "Url    = " << _url << std::endl;
	std::cout << "Protocol = " << _protocol << std::endl;
	std::cout << "Boundary = " << _boundary << std::endl;
	std::cout << "Name = " << _name << std::endl;
	std::cout << "Filename = " << _file_name << std::endl;
	std::cout << "Type = " << _type << std::endl;
}