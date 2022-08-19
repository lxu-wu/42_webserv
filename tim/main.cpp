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
	
	
	/* 
	location /loc1
root ./website/bla
index foo.html
allowed_methods GET
allowed_methods POST

location /loc2
root ./website/bla
index foo.html

server
listen 80
server_name yolo.be
root ./website/ressources
index index_bis.html
client_max_body_size 9000000000
	*/
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
