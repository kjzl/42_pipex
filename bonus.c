/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 12:41:11 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 13:20:35 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	read_write_heredoc(int here_doc, t_str_slice delimiter,
		t_str_slice line, int stdin_dup)
{
	while (true)
	{
		write(2, "> ", 2);
		line = cstr_view(get_next_line(stdin_dup));
		if (!line.str)
			break ;
		if ((strsl_ends_with(line, cstr_slice("\n", 1)) && ft_memcmp(line.str,
					delimiter.str, delimiter.len) == 0) || strsl_eq(line,
				delimiter))
			break ;
		if (write(here_doc, line.str, line.len) != (ssize_t)line.len)
		{
			perror(HERE_DOC_FILE);
			unlink(HERE_DOC_FILE);
			close(here_doc);
			close(stdin_dup);
			free((char *)line.str);
			free(get_next_line(stdin_dup));
			exit(1);
		}
	}
	close(stdin_dup);
	close(here_doc);
	free((char *)line.str);
	free(get_next_line(stdin_dup));
}

/// @return The fd of the temporary file containing the here document.
/// @warning This will exit the program if something goes wrong.
static int	create_temp_heredoc(t_str_slice delimiter)
{
	int			stdin_dup;
	int			here_doc;

	here_doc = open(HERE_DOC_FILE, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (here_doc == -1)
	{
		perror(HERE_DOC_FILE);
		exit(1);
	}
	stdin_dup = dup(0);
	if (stdin_dup == -1)
	{
		unlink(HERE_DOC_FILE);
		close(here_doc);
		exit(status_err_dup_fd);
	}
	read_write_heredoc(here_doc, delimiter, strsl_null(), stdin_dup);
	here_doc = open(HERE_DOC_FILE, O_RDONLY);
	unlink(HERE_DOC_FILE);
	return (here_doc);
}

static void	add_last_cmd_bonus(int cmds_count, char *const *argv, char *const *envp,
		t_vec *out)
{
	if (cmd_new_with_file_out_append(argv[2 + cmds_count - 1], argv[2
				+ cmds_count], envp, vec_get_next_uninit(out)))
	{
		out->len++;
		if (out->len > 1 && !cmds_pipe_io(vec_get_at(out, out->len - 2),
				vec_get_last(out)))
		{
			vec_destroy(out, cmd_del);
			exit(status_err_last_cmd_prep);
		}
		return ;
	}
	vec_destroy(out, cmd_del);
	exit(status_err_last_cmd_prep);
}

void	bonus_argv_to_cmds(int argc, char *const *argv, char *const *envp,
		t_vec *out)
{
	t_cmd	*cmds;
	int		heredoc;

	if (ft_strcmp(argv[1], "here_doc") != 0)
		return (argv_to_cmds(argc, argv, envp, out));
	if (argc < 6)
	{
		ft_putendl_fd("Usage: ./pipex here_doc delimiter cmd1 cmd2"
			"[cmd...] file_out",
			2);
		exit(1);
	}
	argv++;
	heredoc = create_temp_heredoc(cstr_view(argv[1]));
	*out = vec_empty_with_capacity(sizeof(t_cmd), argc - 4);
	if (out->mem_err)
	{
		close(heredoc);
		exit(status_err_malloc);
	}
	cmds = vec_get(out);
	if (cmd_new_with_heredoc_in(argv[2], heredoc, envp, cmds))
		out->len++;
	add_bonus_cmds(argc - 4, argv, envp, out);
	add_last_cmd_bonus(argc - 4, argv, envp, out);
}
