/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 14:39:34 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/17 14:57:57 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "john.hpp"
#define PORT 8080

/* PARSING:
- ARGC == 2
- .conf file
- Directives exist
- TO DO: Directives in between sever brackets.
- TO DO: Stock directives in data name.empty()
*/

int main(int argc, char **argv)
{
	Conf data;

	parsing(argc, argv, data);
	
	// Server serv;

    // serv.setup(PORT);
    // serv.start();
	
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
