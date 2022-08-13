/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_conf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 15:14:35 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/13 17:46:41 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"

void parsing(int argc, char **argv)
{
	Conf data;
	
	parse_basic(argc, argv);
	read_file(argv, &data);
	
}

/* Check if file is good to use */
void parse_basic(int argc, char **argv)
{
	if (argc != 2)
		exit(1);

	std::ifstream file(argv[1]);

	if (!file)
	{
		cerr << "Error: file could not be opened" << endl;
		exit (1);
	}
	else
	{
		std::string name = std::string(argv[1]);
		
		if (name.find(".conf") == std::string::npos) //npos == -1 in size_t
		{
			cerr << "Error: file is not .conf" << endl;
			exit (1);
		}
	}
	
}

/* Fill data will all read lines */
void read_file(char **argv, Conf *data)
{
	std::ifstream file(argv[1]);
	std::string output;

	while (std::getline(file, output))
	{
		data->get_file.insert();
	}
}

// void parse_conf(char **argv)
// {
// 	std::ifstream file(argv[1]);
// 	std::string output;
	
// 	while ( std::getline(file, output) )
// 	{
//       	cout << output << endl;
// 	}
// }