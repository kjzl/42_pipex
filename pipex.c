/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 16:36:07 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 13:15:05 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	add_last_cmd(int cmds_count, char *const *argv, char *const *envp,
		t_vec *out)
{
	if (cmd_new_with_file_out(argv[2 + cmds_count - 1], argv[2 + cmds_count],
			envp, vec_get_next_uninit(out)))
	{
		out->len++;
		return ;
	}
	vec_destroy(out, cmd_del);
	exit(status_err_last_cmd_prep);
}

/// @brief Handles all commands other than the first and the last
/// @param cmds_count
/// @param argv
/// @param envp
/// @param out
void	add_bonus_cmds(int cmds_count, char *const *argv, char *const *envp,
		t_vec *out)
{
	int	cmd_i;

	cmd_i = 1;
	while (cmd_i < cmds_count - 1)
	{
		if (cmd_new(argv[2 + cmd_i], envp, vec_get_next_uninit(out)))
		{
			out->len++;
			if (out->len > 1 && !cmds_pipe_io(vec_get_at(out, out->len - 2),
					vec_get_last(out)))
			{
				vec_destroy(out, cmd_del);
				exit(status_err_dup_fd);
			}
		}
		cmd_i++;
	}
}

/// @brief Creates the pipex command-chain from the given program arguments.
/// @param argc The number of arguments.
/// @param argv The arguments.
/// @return A vector of linked commands.
/// @warning This will exit the program on error,
/// make sure not to allocate heap memory before calling.
/// @warning Make sure argc is at least 5 before calling.
void	argv_to_cmds(int argc, char *const *argv, char *const *envp, t_vec *out)
{
	t_cmd	*cmds;
	int		cmds_count;

	*out = vec_empty_with_capacity(sizeof(t_cmd), argc - 3);
	if (out->mem_err)
		exit(status_err_malloc);
	cmds_count = argc - 3;
	cmds = vec_get(out);
	if (cmd_new_with_file_in(argv[2], argv[1], envp, cmds))
		out->len++;
	add_bonus_cmds(cmds_count, argv, envp, out);
	add_last_cmd(cmds_count, argv, envp, out);
}

#ifdef BONUS

int	main(int argc, char *const *argv, char *const *envp)
{
	t_vec	cmds;
	int		status;

	if (argc < 5)
	{
		ft_putendl_fd("Usage: ./pipex <file_in/[here_doc <delimiter>]>"
			"cmd1 cmd2 [cmd...] file_out",
			2);
		exit(1);
	}
	bonus_argv_to_cmds(argc, argv, envp, &cmds);
	status = cmds_exec_and_wait_all(&cmds, envp);
	if (status == 1)
	{
		status = ((t_cmd *)vec_get_last(&cmds))->status;
		vec_destroy(&cmds, cmd_del);
		return (status);
	}
	vec_destroy(&cmds, cmd_del);
	if (status == -1)
		return (status_child_err_exec);
	if (status == 0)
		return (1);
}

#else

int	main(int argc, char *const *argv, char *const *envp)
{
	t_vec	cmds;
	int		status;

	if (argc != 5)
	{
		ft_putendl_fd("Usage: ./pipex file_in cmd1 cmd2 file_out", 2);
		exit(1);
	}
	argv_to_cmds(argc, argv, envp, &cmds);
	status = cmds_exec_and_wait_all(&cmds, envp);
	if (status == 1)
	{
		status = ((t_cmd *)vec_get_last(&cmds))->status;
		vec_destroy(&cmds, cmd_del);
		return (status);
	}
	vec_destroy(&cmds, cmd_del);
	if (status == -1)
		return (status_child_err_exec);
	if (status == 0)
		return (1);
}

#endif
