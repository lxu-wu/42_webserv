/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tim_requete.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:03 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/30 15:46:46 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIM_REQUETE_HPP
# define TIM_REQUETE_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <iterator>
# include <string>

class Tim_requete
{
	public:
		Tim_requete(std::string requete);
		~Tim_requete() {};

		/* Getters */
		std::string getMethod() const	{return _method;};
		std::string getUrl() const		{return _url;};
		std::string getProtocol() const	{return _protocol;};
		std::string getBoundary() const	{return _boundary;};

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
		std::string							_body;//content body
		size_t								_len;
		std::map<std::string, std::string>	_request;
		std::map<std::string, std::string>	_text;//if more than 1 request + no content type
};

#endif