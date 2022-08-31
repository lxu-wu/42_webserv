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
	_full_body = requete;
	std::stringstream ss;
	std::string token, line, key;
    ss << requete;

    ss >> this->_method;
    ss >> this->_url;
	ss >> this->_protocol;
	while (ss >> token)
	{
		if (token.find("boundary=") != std::string::npos)
		{
			_boundary = token.substr(token.find("boundary=") + 9);
		}
		else if (token == "Content-Lenght:")
		{
			ss >> token;
			_len = atoi(token.c_str());
		}
		else if (!_boundary.empty() && token.find(_boundary) != std::string::npos) //begin of body
		{
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
	if (!line.empty())
	{
		line.pop_back();
		_header.insert(std::pair<std::string, std::string>(key, line));
	}
	// std::map<std::string, std::string>::iterator it = _request.begin();
	// std::map<std::string, std::string>::iterator it_end = _request.end();
	// while (it != it_end)
	// {
	// 	std::cout << it->first << " = " << it->second << std::endl;
	// 	it++;
	// }
}

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
	std::string body;
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
		
		if (token.find("filename=") != std::string::npos)
		{
			while (!token.empty() && token.find(_boundary) == std::string::npos)
			{
				body += token;
				body += " ";
				ss >> token;
			}
			_text.insert(std::pair<std::string, std::string>(_name, body));
			body.empty();
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
				body += token;
				body += " ";
				ss >> token;
			}
			_body = body;
			body.empty();
		}
	}
	std::cout << "------NAME == " << _name << std::endl;
	std::cout << "------FNAME == " << _file_name << std::endl;
	std::cout << "------BODY == " << _body << std::endl;
	std::cout << "------LEN == " << _len << std::endl;
}
