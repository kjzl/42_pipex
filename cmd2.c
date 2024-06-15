/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 07:25:04 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 12:33:58 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

t_bool	cmd_new_with_heredoc_in(const char *cmd_str, int fd, char *const *envp,
		t_cmd *out)
{
	cmd_init(out);
	out->in[0] = fd;
	if (out->in[0] == -1)
	{
		perror(HERE_DOC_FILE);
		return (false);
	}
	if (pipe(out->out) == -1)
	{
		close_pipe(out->in);
		return (false);
	}
	out->cmd = cmd_str_to_argv(cstr_view(cmd_str), envp);
	if (out->cmd == 0)
	{
		close_pipe(out->in);
		close_pipe(out->out);
		return (false);
	}
	return (true);
}

t_bool	cmd_new_with_file_in(const char *cmd_str, const char *file_in,
		char *const *envp, t_cmd *out)
{
	cmd_init(out);
	out->in[0] = open(file_in, O_RDONLY);
	if (out->in[0] == -1)
	{
		perror(file_in);
		return (false);
	}
	if (pipe(out->out) == -1)
	{
		close_pipe(out->in);
		return (false);
	}
	out->cmd = cmd_str_to_argv(cstr_view(cmd_str), envp);
	if (out->cmd == 0)
	{
		close_pipe(out->in);
		close_pipe(out->out);
		return (false);
	}
	return (true);
}

void	cmd_del(void *_cmd)
{
	char	**cmd_ptr;
	t_cmd	*cmd;

	cmd = _cmd;
	if (cmd->cmd)
	{
		cmd_ptr = cmd->cmd;
		while (*cmd_ptr)
			free(*cmd_ptr++);
		free(cmd->cmd);
		cmd->cmd = 0;
	}
	close_pipe(cmd->in);
	close_pipe(cmd->out);
	cmd->pid = -1;
	cmd->status = -1;
}

void	cmds_close_pipes(t_vec *cmds)
{
	t_cmd	*cmd;
	size_t	i;

	i = 0;
	cmd = vec_get(cmds);
	while (i++ < cmds->len)
	{
		close_pipe(cmd->in);
		close_pipe(cmd->out);
		cmd++;
	}
}

/// @brief Executes a command in a fork with redirected input and output.
/// @param cmd Command to execute.
/// @return PID of the child process or -1 if the fork failed
/// or 0 if the child failed to execute the command.
pid_t	cmd_exec(t_cmd *cmd, t_vec *cmds, char *const *envp)
{
	t_bool	ok;
	pid_t	pid;

	pid = fork();
	ok = true;
	if (pid == 0)
	{
		if (cmd->in[0] != -1)
			ok = dup2(cmd->in[0], 0) != -1;
		else
			ok = false;
		if (ok && cmd->out[1] != -1)
			ok = dup2(cmd->out[1], 1) != -1;
		cmds_close_pipes(cmds);
		if (ok)
			execve(cmd->cmd[0], cmd->cmd, envp);
	}
	else
		cmd->pid = pid;
	return (pid);
}
