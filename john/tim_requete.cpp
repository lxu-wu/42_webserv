/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tim_requete.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/22 16:18:30 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/22 16:43:26 by tmartial         ###   ########.fr       */
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