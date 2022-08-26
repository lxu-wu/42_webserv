/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tim_requete.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecorte42 <jdecorte42@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:30 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/23 11:36:49 by jdecorte42       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tim_requete.hpp"

Tim_requete::Tim_requete(std::string requete)
{
	std::stringstream ss;
    ss << requete;

    ss >> this->_method;
    ss >> this->_url;
}

bool Tim_requete::check_tim()
{
	if (_method != "POST" && _method != "GET" && _method != "DELETE")
		return false;
	return true;
}