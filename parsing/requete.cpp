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
	//std::cout << "Before check " << std::endl;
	std::stringstream ss;
	_request = requete;
	_len = 0;
    ss << requete;
    ss >> this->_method;
    ss >> this->_url;
	ss >> this->_protocol;
	make_query();
	if (_method == "GET")
	{
		make_GET(ss);
	}
	else if (_method == "POST")
	{
		make_POST(ss);
		//std::cout << "Body      = " << this->_body << std::endl;
		//std::cout << "Full Body = " << this->_full_body << std::endl;
		//std::cout << "Request   = " << this->_request << std::endl;

	}
	//std::cout << "After check " << std::endl;
}

/* Check if request is good */
bool Requete::check_tim()
{
	if (_method != "POST" && _method != "GET" && _method != "DELETE")
		return false;
	if (_protocol != "HTTP/1.1")
		return false;
	return true;
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
				ss >> token;
			}
			_text.insert(std::pair<std::string, std::string>(_name, temp));
			_body += temp;
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
			temp.clear();
		}
	}
}

/* Make post request */
void Requete::make_POST(std::stringstream& ss)
{
	std::string token, line, key;

	while (ss >> token)
	{
		if (token.find("boundary=") != std::string::npos)
		{
			_boundary = token.substr(token.find("boundary=") + 9);
		}
		else if (token == "Content-Length:")
		{
			key = token;
			ss >> token;
			_header.insert(std::pair<std::string, std::string>(key, token));
			_len = atoi(token.c_str());
			key.clear();
		}
		else if (!_boundary.empty() && token.find(_boundary) != std::string::npos) //begin of body
		{
			//std::cout << "Body Making" << std::endl;
			make_body(ss, token);
			break;
		}
		else if (token.back() == ':')
		{
			if (!key.empty() && key != token)
			{
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
		line.pop_back();
		_header.insert(std::pair<std::string, std::string>(key, line));
	}
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
	if (!line.empty())
	{
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
	_full_body = _body;
	//std::cout << "request len = " << _request.length() << std::endl;
	//std::cout << "len = " << _len << std::endl;
	//_full_body = _request.substr(_request.length() - _len);
}