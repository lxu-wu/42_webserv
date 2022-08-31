/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requete.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:03 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/31 14:45:43 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUETE_HPP
# define REQUETE_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <iterator>
# include <string>

class Requete
{
	public:
		Requete(std::string requete);
		~Requete() {};

		/* Getters */
		std::string							getMethod() const	{return _method;};
		std::string							getUrl() const		{return _url;};
		std::string							getProtocol() const	{return _protocol;};
		std::string							getBoundary() const	{return _boundary;};
		std::string							getName() const		{return _boundary;};
		std::string							getFileName() const	{return _file_name;};
		std::string							getType() const		{return _type;};
		std::string							getBody() const		{return _body;};
		size_t								getLen() const		{return _len;};
		std::string							getFullBody() const	{return _full_body;};
		std::map<std::string, std::string>	getHeader() const	{return _header;};
		std::map<std::string, std::string>	getText() const		{return _text;};

		/* Functions */
		bool check_tim();
		void make_body(std::stringstream& ss, std::string token);
		
	protected:
		std::string							_method;
		std::string							_url;
		std::string							_protocol;
		std::string							_boundary;
		std::string							_name;//content name
		std::string							_file_name;
		std::string							_type;//content type
		std::string							_body;//content body without boundary
		size_t								_len;
		std::string							_full_body;//for LXU WU
		std::map<std::string, std::string>	_header;//
		std::map<std::string, std::string>	_text;//if more than 1 request + no content type first arg = name && second = body
};

#endif