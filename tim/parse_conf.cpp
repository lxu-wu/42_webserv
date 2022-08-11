/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_conf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 15:14:35 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/11 15:16:56 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"

void parsing(int argc, char **argv)
{
	if (argc != 2)
		exit(1);
	
	std::ifstream file(argv[1]);
	std::string output;

	if (!file)
	{
		cerr << "Error: file could not be opened" << endl;
		exit (1);
	}
	else
	{
		while ( std::getline(file, output) )
		{
      		cout << output << endl;
		}
	}
}