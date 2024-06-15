/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 11:09:54 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 12:49:49 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <sys/stat.h>

static t_bool	set_stdin_to_empty_pipe(t_cmd *cmd)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (false);
	close(pipe_fd[1]);
	cmd->in[0] = pipe_fd[0];
	return (true);
}

t_bool	cmd_new_with_file_out_append(const char *cmd_str,
		const char *file_out, char *const *envp, t_cmd *out)
{
	cmd_init(out);
	set_stdin_to_empty_pipe(out);
	out->out[1] = open(file_out, O_WRONLY | O_APPEND | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (out->out[1] == -1)
	{
		perror(file_out);
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

t_bool	cmd_new_with_file_out(const char *cmd_str, const char *file_out,
		char *const *envp, t_cmd *out)
{
	cmd_init(out);
	set_stdin_to_empty_pipe(out);
	out->out[1] = open(file_out, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (out->out[1] == -1)
	{
		perror(file_out);
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

/// @brief Creates a new command with an out pipe/file and an optional in file.
/// @param cmd Command string (i.e. "ls -l").
/// @param file_in Input file or null.
/// @param file_out Output file or null.
/// @param out Command struct to fill.
/// @return True if successful, false otherwise.
t_bool	cmd_new(const char *cmd_str, char *const *envp, t_cmd *out)
{
	cmd_init(out);
	set_stdin_to_empty_pipe(out);
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

/// @brief Connects the output of one command to the input of another.
/// @param from Command with to be linked output pipe.
/// @param to Command with to be linked input pipe.
/// @return True if successful, false otherwise.
t_bool	cmds_pipe_io(t_cmd *from, t_cmd *to)
{
	int	tmp;

	if (from->out[0] == -1)
		return (true);
	tmp = to->in[0];
	to->in[0] = dup(from->out[0]);
	if (to->in[0] == -1)
	{
		to->in[0] = tmp;
		return (false);
	}
	if (tmp != -1)
		close(tmp);
	if (to->in[1] != -1)
	{
		close(to->in[1]);
		to->in[1] = -1;
	}
	return (true);
}
