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

void parsing(int argc, char **argv, Conf &data)
{
	parse_basic(argc, argv); //ARGC && .conf
	data.read_file(argv[1]);
	data.check_brackets(); //Num Brackets
	data.check_directive();//All lines are directives && good num of element
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

/* Count words in a sentence */ 
int count_words(std::string sentence)
{
	int ret = 0, i = 0;

	while (sentence[i + 1])
	{
		if (!isspace(sentence[i]) && isspace(sentence[i + 1]))
			ret++;
		i++;
	}
	if (!isspace(sentence[i]))
		ret++;
	return (ret);
}

/* Retun first word from line */
std::string ft_first_word(std::string line)
{
	int i = 0, j = 0;
	while (isspace(line[i]) && line[i])
		i++;
	j = i;
	while (!isspace(line[j]) && line[j])
		j++;
	return (line.substr(i, j - i));
}
