/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalevesq <jalevesq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 09:23:24 by jalevesq          #+#    #+#             */
/*   Updated: 2023/04/04 18:38:08 by jalevesq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Error func good ?
t_token	*ft_fill_list(char **split_input)
{
	t_token	*token;
	t_token	*tmp;
	int		i;

	i = 0;
	token = new_node(split_input[i++]);
	token->prev = NULL;
	tmp = token;
	while (split_input[i])
	{
		tmp->next = new_node(split_input[i++]);
		if (!tmp->next)
			ft_end_list(token);
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}
	return (token);
}

void	ft_assign_type(t_token *token)
{
	t_token	*temp;

	temp = token;
	while (temp != NULL)
	{
		if (ft_strncmp(temp->str, "<<", 2) == 0)
			temp->type = LESS_LESS;
		else if (ft_strncmp(temp->str, ">>", 2) == 0)
			temp->type = GREAT_GREAT;
		else if (ft_strncmp(temp->str, "<", 1) == 0)
			temp->type = LESS;
		else if (ft_strncmp(temp->str, ">", 1) == 0)
			temp->type = GREAT;
		else if (ft_strncmp(temp->str, "|", 1) == 0)
			temp->type = PIPE;
		else
			temp->type = CMD;
		if (temp->type == CMD && (temp->prev && (temp->prev->type == GREAT
			|| temp->prev->type == GREAT_GREAT 
			|| temp->prev->type == LESS_LESS || temp->prev->type == LESS)))
			temp->type = FILE;
		temp = temp->next;
	}
}

char	*ft_redifine(char *mots, char *str, char sign)
{
	int		a;
	int		b;
	int		c;
	int		tmp;
	char	*new;

	a = 0;
	b = 0;
	c = 0;
	while (str[a] != sign && str[a])
		a++;
	c = a;
	while ((str[c] != 32 && ft_wake_word(str[c]) == 0 && str[c] != 39
			&& str[c] != 34) && str[c])
	{
		c++;
		b++;
	}
	tmp = c;
	c = (ft_strlen(str) - b + ft_strlen(mots));
	new = malloc((sizeof(char) * c) + 1);
	b = 0;
	while (b < a)
	{
		new[b] = str[b];
		b++;
	}
	c = ft_strlen(mots);
	a = 0;
	while (a < c)
	{
		new[b] = mots[a];
		b++;
		a++;
	}
	while (str[tmp])
	{
		new[b] = str[tmp];
		b++;
		tmp++;
	}
	return (new);
}

char	*ft_find_var(char *str, char **envp, char *vars)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(vars, envp[i], ft_strlen(vars)) == 0)
		{
			vars = ft_strtrim(envp[i], vars);
			// printf("vars = %s\n", vars);
			str = ft_redifine(vars, str, '$');
			return (str);
		}
		i++;
	}
	str = ft_redifine("", str, '$');
	return (str);
}

int	ft_where(char *str, char c, int position)
{
	int	i;

	i = position;
	while (str[i])
	{
		if (str[i] == c)
			return (i + 1);
		i++;
	}
	return (-1);
}

char	*ft_take_var(char *str, int position)
{
	int		i;
	int		nb;
	char	*var;

	i = position;
	while (str[i])
	{
		if (ft_wake_word(str[i]) == 1 || str[i] == ' ' || str[i] == 39
			|| str[i] == 34)
			break ;
		i++;
	}
	nb = i - position;
	var = malloc((sizeof(char) * nb) + 2);
	i = 0;
	while (i < nb)
	{
		var[i] = str[position];
		i++;
		position++;
	}
	var[i] = '=';
	return (var);
}

char	*ft_chevronparsing(char *str, int i)
{
	// int	j;
	// j = 0;
	while (str[i] == 32)
		i++;
	while ((ft_wake_word(str[i]) > 0 || str[i] == 32) && str[i])
		i++;
	while ((ft_wake_word(str[i]) == 0 && str[i] != 32 && str[i] != 29)
		&& str[i])
		i++;
	if (i < (int)ft_strlen(str) && ft_wake_word(str[i]) == 0)
		str[i] = 29;
	return (str);
}

char	*ft_write_cut(char *str)
{
	int		i;
	int		j;
	int		count;
	char	*output;

	// int		temoins;
	i = 0;
	j = 0;
	count = ft_count_parsing(str);
	// printf("count = %d\n", count);
	output = ft_calloc(sizeof(char), (ft_strlen(str) + count) + 1);
	// temoins = 0;
	str[ft_strlen(str)] = 0;
	while (str[i])
	{
		if (ft_wake_word(str[i]) >= 1)
		{
			if (ft_wake_word(str[i]) == 2)
				str = ft_chevronparsing(str, i + 1);
			if (i > 0)
			{
				output[j] = 29;
				j++;
			}
			while (ft_wake_word(str[i]) >= 1 || str[i] == 32)
			{
				if (ft_wake_word(str[i]) == 2)
					str = ft_chevronparsing(str, i + 1);
				output[j] = str[i];
				j++;
				i++;
			}
			output[j] = 29;
		}
		else
		{
			output[j] = str[i];
			i++;
		}
		j++;
	}
	return (output);
}

void	ft_insertNode(t_token *head, int character, int position)
{
	t_token	*newNode;
	t_token	*currentNode;
	int		i;
	char	*newData;

	newNode = (t_token *)malloc(sizeof(t_token));
	newData = ft_calloc(sizeof(char), 4);
	if (character == 2 || character == 4)
		newData[0] = '<';
	if (character == 3 || character == 5)
		newData[0] = '>';
	if (character == 4)
		newData[1] = '<';
	if (character == 5)
		newData[1] = '>';
	newNode->str = newData;
	currentNode = head;
	i = 1;
	while (i < position && currentNode != NULL)
	{
		currentNode = currentNode->next;
		i++;
	}
	if (currentNode == NULL)
	{
		printf("error link list in parser\n");
		return ;
	}
	newNode->next = currentNode->next;
	currentNode->next = newNode;
	newNode->prev = currentNode;
	if (newNode->next != NULL)
		newNode->next->prev = newNode;
}

char	*ft_resize(char *str)
{
	char	*newstr;

	free(str);
	newstr = ft_calloc(2, sizeof(char));
	newstr[0] = '|';
	return (newstr);
	return (newstr);
}

void	ft_parser(t_init *var)
{
	t_token *token;
	char **split;
	int result;
	// int j;
	// int i;

	var->input = ft_strtrim(var->input, " ");
	var->input = ft_write_cut(var->input);
	split = ft_split_parser(var->input, 29);
	token = ft_fill_list(split);
	ft_free_double(split);
	t_token *tmp = token;
	// i = 1;
	// j = 1;
	while (tmp)
	{
		tmp->str = ft_strtrim(tmp->str, " ");
		if (ft_syntax(tmp->str) == 1)
			return ;
		if (!tmp->next)
		{
			if (ft_wake_word(tmp->str[0]) > 0)
			{
				// 	printf("next = %c\n", tmp->str[0]);
				ft_error_syntax(tmp->str);
				return ;
			}
		}
		tmp->str = ft_guillemet(tmp->str, var);
		tmp->str = ft_strtrim(tmp->str, " ");
		if (ft_syntax(tmp->str) >= 2)
		{
			result = ft_syntax(tmp->str);
			// tmp->str[1] = ft_resize(tmp->str);
			tmp->str[1] = 0;
			// printf("result = %d\n", result);
			ft_insertNode(tmp, result, 1);
		}
		// mettre le code pour le | > ici
		// printf("%s\n", tmp->str);
		// pour print les valeurs dans linked list et faire des tests.
		tmp = tmp->next;
		// i++;
	}
	ft_assign_type(token);
	ft_executor(token, var->envp);
	ft_free_list(token);
}