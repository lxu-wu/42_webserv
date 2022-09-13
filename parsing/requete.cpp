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
	// std::ofstream MyFile("my_input.txt");
	// MyFile << requete;
	// MyFile.close();
	_char_request = requete;
	std::stringstream ss(requete);
	_request = requete;
	_len = 0;
    ss >> this->_method;
    ss >> this->_url;
	ss >> this->_protocol;
	make_query();
	if (_method == "GET")
		make_GET(ss);
	else if (_method == "POST")
		make_POST(ss);
}

Requete::~Requete()
{
	_header.clear();
	_text.clear();
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
	size_t pos = _request.find("\r\n\r\n") + 4, pos_body = 0;
	std::string save = _request.substr(pos);

	/* ONE BODY */
	if (save.find("filename=") != std::string::npos)
	{
		ss >> token;//boundary
		ss >> token;//Content-Disposition:
		ss >> token;//form-data;
		ss >> token;//name="file1";
		_name = token.substr(token.find("="));
		if (_name.back() == ';')
			_name.pop_back();//remove ;
		_name.pop_back();//remove " at end
		_name.erase(0, 2);//remove " at begin
		ss >> token;

		_file_name = token.substr(token.find("="));
		_file_name.pop_back();//remove " at end
		_file_name.erase(0, 2);//remove " at begin
		ss >> token;
		if (token.find("Content-Type:") != std::string::npos)
		{
			ss >> token;
			_type = token;
		}
	}
	else /* MORE BODYS */
	{
		ss >> token;
		while (!save.empty())
		{
			_text.push_back("NewFile");
			ss >> token;//Content-Disposition:
			ss >> token;//form-data;
			_text.push_back(token);
			ss >> token;//name="file1";
			save.erase(0, save.find(token) + token.length() + 4);
			_name = token.substr(token.find("="));
			if (_name.back() == ';')
				_name.pop_back();//remove ;
			_name.pop_back();//remove " at end
			_name.erase(0, 2);//remove " at begin
			_text.push_back(_name);
			while (token.find(_boundary) == std::string::npos) //Advance till next boundary
				ss >> token;
			_body = save.substr(0, save.find(_boundary) - 3);
			_text.push_back(_body);
			_body.clear();
			if (save[save.find(_boundary) + _boundary.size()] == '-')//End of body
				break;
		}
		//print_text();
	}

	/* Make full body */
	pos_body = pos;
	while (pos < _len + pos_body)
	{
		_body += _char_request[pos];
		pos++;
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
			if (!key.empty() && key != token)
			{
				if (!line.empty())
					line.pop_back();//remove space
				if (!line.empty())
				{
					_header.insert(std::pair<std::string, std::string>(key, line));
				}
				else
				{
					_header.insert(std::pair<std::string, std::string>(key, token));
				}
			}
			size_t pos_header = pos;
			while (pos < _len + pos_header - 1)// || _request[pos + 1]
			{
				_full_body += _char_request[pos];
				pos++;
			}
			if (_boundary.empty())
				_body = _full_body;
			else
			{
				//make_body(ss, token); //John Did it
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

/* Print text */
void Requete::print_text()
{
	std::vector<std::string>::iterator it = _text.begin();
	while (it != _text.end())
	{
		std::cout << "It = " << *it << std::endl;
		it++;
	}
}