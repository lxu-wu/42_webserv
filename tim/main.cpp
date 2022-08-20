/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 14:39:34 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/19 14:58:42 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv)
{
	Conf data;

	try
	{
		parsing(argc, argv, data);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << endl;
		return (1);
	}
	data.print_all_data();
	
	return 0;	
}
