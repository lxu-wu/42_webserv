/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 14:39:34 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/11 15:17:49 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv)
{
	Conf data;

	parsing(argc, argv, data);

	// std::vector<std::string> test = data.get_file();
	// std::vector<std::string>::iterator it = test.begin();
	// std::vector<std::string>::iterator it_end = test.end();
	// while (it != it_end)
	// {
	// 	cout << "it = " << *it << endl;
	// 	it++;
	// }
	
	return 0;	
}
