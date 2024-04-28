/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 11:06:18 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 07:29:53 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	cmd_init(t_cmd *out)
{
	out->cmd = 0;
	out->status = -1;
	out->pid = -1;
	out->in[0] = -1;
	out->in[1] = -1;
	out->out[0] = -1;
	out->out[1] = -1;
}

void	close_pipe(int *pipe)
{
	if (pipe[0] != -1)
		close(pipe[0]);
	pipe[0] = -1;
	if (pipe[1] != -1)
		close(pipe[1]);
	pipe[1] = -1;
}

static t_bool	wait_for_cmd_and_get_exit_status(t_cmd *cmd)
{
	if (waitpid(cmd->pid, &cmd->status, 0) == -1)
		return (false);
	cmd->status = WEXITSTATUS(cmd->status);
	return (true);
}

/// @brief
/// @param cmds
/// @return
/// 1 if successful
/// 0 if unsuccessful
/// -1 if current process is child and program should exit without cleanup
int	cmds_exec_and_wait_all(t_vec *cmds, char *const *envp)
{
	pid_t	pid;
	t_cmd	*cmd;
	t_bool	ok;
	size_t	i;

	i = 0;
	ok = true;
	while (i < cmds->len)
	{
		cmd = vec_get_at(cmds, i++);
		pid = cmd_exec(cmd, cmds, envp);
		if (pid == 0)
			return (-1);
		if (pid > 0)
			continue ;
		ok = false;
		break ;
	}
	cmds_close_pipes(cmds);
	while (i-- > 0)
	{
		if (!wait_for_cmd_and_get_exit_status(vec_get_at(cmds, i)))
			ok = false;
	}
	return (ok);
}
