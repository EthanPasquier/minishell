/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalevesq <jalevesq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 10:39:17 by jalevesq          #+#    #+#             */
/*   Updated: 2023/03/28 16:18:21 by jalevesq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void ft_child_pipe(t_child *c, t_token *t, int *fd)
{
	int fd2;
	t_token *tmp;

	tmp = t;
	fd2 = -1;
	if (t->next && t->next->type == GREAT)
	{
		tmp = tmp->next;
		if (tmp->next->next && tmp->next->next->type == GREAT)
		{
			while(tmp && tmp->type == GREAT)
			{
				fd2 = -1;
				fprintf(stderr, "TEST\n");
				fd2 = open(tmp->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0640);
				if (tmp->next && tmp->next->next && tmp->next->type == FILE && tmp->next->next->type == GREAT)
				{
					close(fd2);
					tmp = tmp->next->next;
				}
				else
					break;
			}	
		}
		else
		{
			fd2 = open(tmp->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0640);
		}
		if (dup2(fd2, STDOUT) == -1)
			ft_error(1); // temp, bad exit
		close(fd2);
		// fprintf(stderr, "fd2, STDOUT\n");
	}
	if (t->prev && t->prev->type == PIPE && t->next && t->next->type == PIPE)
	{
		if (dup2(fd[c->j - 1], STDIN) == -1)
			ft_error(1); // temp, bad exit
		if (dup2(fd[c->j], STDOUT) == -1)
			ft_error(1); // temp, bad exit//
		// fprintf(stderr, "ELSE - fd[%d - 1], STDIN\n", c->j);
		// fprintf(stderr, "ELSE - fd[%d], STDOUT\n", c->j);
	}
	else if (c->i == 0 && t->next && t->next->type == PIPE)
	{
		if (dup2(fd[c->j], STDOUT) == -1)
			ft_error(1); // temp, bad exit
		// fprintf(stderr, "fd[%d], STDOUT\n", c->j);
	}
	else if (t->prev && t->prev->type == PIPE)
	{
		if (dup2(fd[c->j - 1], STDIN) == -1)
			ft_error(1); // temp, bad exit
		// fprintf(stderr, "fd[%d - 1], STDIN\n", c->j);
	}

}

void	ft_exec_child(t_child *child, t_token *token, int *fd)
{
	child->j = child->i * 2; // j = pipfd[1] et j+1 = pipfd[0]
	// if ((token->next && token->next->type == PIPE)
	// 	|| (token->prev && token->prev->type == PIPE))
	// 	ft_child_pipe(child, token, fd);
	// if (token->next && token->next->type == GREAT)
	// 	ft_child_redirection(child, token, fd);
	ft_child_pipe(child, token, fd);
	ft_close_child(fd, child->cmd_nbr);
	execve(child->cmd_path, child->cmd, child->envp);
	ft_error(1);
}

void	ft_exec_command(t_token *t, t_child *c, int *fd, int *pid)
{
	t_token	*tmp;

	tmp = t;
	while (tmp && tmp->type != CMD)
		tmp = tmp->next;
	while (c->i < c->cmd_nbr)
	{
		if (t->type == CMD)
		{
			c->cmd = ft_split(tmp->str, ' ');
			c->cmd_path = find_cmd_path(c->cmd, c->all_path);
			if (c->cmd_path != NULL)
			{
				pid[c->i] = fork();
				if (pid[c->i] < 0)
					ft_error(1);
				else if (pid[c->i] == 0)
					ft_exec_child(c, tmp, fd); // Change FD for | and < >.
			}
			else
				printf("minishell: %s: command not found\n", c->cmd[0]);
		}
		c->i++;
		ft_free_exec(c->cmd, c->cmd_path);
		while (tmp->next && tmp->next->type != CMD)
			tmp = tmp->next;
		if (tmp && tmp->next)
			tmp = tmp->next;
	}
}

void	ft_command(t_token *token, t_child *child)
{
	int		*fd_array;
	pid_t	*pid;

	fd_array = NULL;
	pid = malloc(sizeof(pid_t) * child->cmd_nbr);
	if (!pid)
	{
		printf("PID ERROR wtf\n");
		exit(EXIT_FAILURE);
	}
	if (child->cmd_nbr > 1)
		fd_array = ft_set_pipe(child);
	ft_exec_command(token, child, fd_array, pid);
	ft_close_child(fd_array, child->cmd_nbr);
	ft_wait(pid, child->cmd_nbr);
	if (fd_array != NULL)
		free(fd_array);
	if (pid != NULL)
		free(pid);
}

void	ft_executor(t_token *token, char **envp)
{
	t_child	child;

	child.cmd_nbr = cmd_counter(token);
	child.all_path = find_path(envp);
	child.envp = envp;
	child.i = 0;

	// if (is_one_command(token) == 0)
	// 	ft_exec_single(&child, envp, token->str);
	// else
	ft_command(token, &child);
	ft_free_double(child.all_path);
}
