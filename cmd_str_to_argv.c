/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_str_to_argv.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 11:22:40 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/21 01:04:52 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static t_bool	strsls_to_cloned_cstrs_and_cleanup(t_vec *split_cmd,
					t_str *cmd_file)
{
	if (!vec_strsls_to_cloned_cstrs(split_cmd, split_cmd))
	{
		vec_destroy(split_cmd, 0);
		str_destroy(cmd_file);
		return (false);
	}
	return (true);
}

/// @brief Transforms a command string into an argv array
/// (while also finding the full path to the cmd).
/// e.g. "ls -l" -> ["/usr/bin/ls", "-l", 0]
/// @param cmd Command string.
/// @return Command string split into an argv cstring array or null.
char	**cmd_str_to_argv(t_str_slice cmd, char *const *envp)
{
	t_vec	split_cmd;
	t_str	cmd_file;
	char	**out;

	split_cmd = strsl_split(cmd, cstr_slice(" ", 1));
	if (split_cmd.len == 0 || split_cmd.mem_err
		|| !find_file_on_path_perror(*(t_str_slice *)vec_get(&split_cmd), envp,
			X_OK, &cmd_file))
	{
		vec_destroy(&split_cmd, 0);
		return (0);
	}
	*(t_str_slice *)vec_get(&split_cmd) = str_view(&cmd_file);
	if (!strsls_to_cloned_cstrs_and_cleanup(&split_cmd, &cmd_file))
		return (0);
	str_destroy(&cmd_file);
	if (!vec_push_null(&split_cmd))
	{
		vec_destroy(&split_cmd, iter_ptr_free);
		return (0);
	}
	out = vec_take(&split_cmd);
	if (!out)
		vec_destroy(&split_cmd, iter_ptr_free);
	return (out);
}
