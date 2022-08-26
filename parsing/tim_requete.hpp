/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tim_requete.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:03 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/22 16:52:49 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIM_REQUETE_HPP
# define TIM_REQUETE_HPP

# include <iostream>
# include <sstream>

class Tim_requete
{
	public:
		Tim_requete(std::string requete);
		~Tim_requete() {};

		/* Getters */
		std::string getMethod() const	{return _method;};
		std::string getUrl() const		{return _url;};

		/* Functions */
		bool check_tim();
		

	private:
		std::string _method;
		std::string _url;
		
};

#endif