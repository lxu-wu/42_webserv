/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tim_requete.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:30 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/26 17:26:16 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tim_requete.hpp"

Tim_requete::Tim_requete(std::string requete)
{
	std::stringstream ss;
	std::string token, line, key;
    ss << requete;

    ss >> this->_method;
    ss >> this->_url;
	ss >> this->_protocol;
	while (ss >> token)
	{
		if (token.back() == ':')
		{
			if (!key.empty() && key != token)
				_request.insert(std::pair<std::string, std::string>(key, line));
			token.pop_back();
			key = token;
			line.clear();
		}
		else
		{
			line += token;
			line += " ";
		}
	}
	_request.insert(std::pair<std::string, std::string>(key, line));
	// std::map<std::string, std::string>::iterator it = _request.begin();
	// std::map<std::string, std::string>::iterator it_end = _request.end();
	// while (it != it_end)
	// {
	// 	std::cout << it->first << " = " << it->second << std::endl;
	// 	it++;
	// }
}

bool Tim_requete::check_tim()
{
	if (_method != "POST" && _method != "GET" && _method != "DELETE")
		return false;
	if (_protocol != "HTTP/1.1")
		return false;
	return true;
}