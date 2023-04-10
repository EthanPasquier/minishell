/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalevesq <jalevesq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 10:42:34 by jalevesq          #+#    #+#             */
/*   Updated: 2023/04/09 21:17:02 by jalevesq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


char **ft_copy_env(char **env)
{
    char **copy_env;
    int i;
    int j;

    i = 0;
    while (env[i])
        i++;
    copy_env = (char **)malloc(sizeof(char*) * (i + 1));
    if (copy_env == NULL)
        return (NULL);
    j = 0;
    while (env[j])
    {
        copy_env[j] = (char *)malloc(sizeof(char) * (ft_strlen(env[j]) + 1));
        if (copy_env[j] == NULL)
            return (NULL);
        ft_strlcpy(copy_env[j], env[j], ft_strlen(env[j]) + 1);
        j++;
    }
    copy_env[j] = NULL;
    return (copy_env);
}

// t_env *ft_copy_env(char **env)
// {
// 	t_env	*envp;
// 	t_env	*tmp;
// 	int		i;

// 	i = 0;
// 	envp = new_node(env[i++]);
// 	tmp = envp;
// 	while (env[i])
// 	{
// 		tmp->next = new_node(env[i++]);
// 		if (!tmp->next)
// 			ft_end_list_env(env);
// 		tmp = tmp->next;
// 	}
// 	return (envp);
// }

// t_init	ft_init(char *input)
// {
// 	t_init	var;

// 	var.input = input;
// 	return (var);
// }

t_token	*new_node(char *str)
{
	t_token	*new_node;

	new_node = malloc(sizeof(t_token));
	if (!new_node)
		return (NULL);
	new_node->next = NULL;
	new_node->type = -1;
	new_node->str = ft_strdup(str); // !!! Free les str dans les struct ET free le ft_split.
	return (new_node);
}

t_env *new_node_env(char *str)
{
	t_env *new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->next = NULL;
	new_node->envp = ft_strdup(str);
	return (new_node);
}