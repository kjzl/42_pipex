/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 10:50:28 by kwurster          #+#    #+#             */
/*   Updated: 2024/06/15 07:37:27 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <wait.h>

typedef enum e_exit_status
{
	status_ok = 0,
	status_err = 1,
	status_err_malloc = 2,
	status_err_dup_fd = 3,
	status_child_err_exec = 77,
	/// bash returns 127 if the last command couldn't be successfully prepared
	status_err_last_cmd_prep = 127,
}			t_exit_status;

typedef struct s_cmd
{
	int		in[2];
	int		out[2];
	char	**cmd;
	pid_t	pid;
	int		status;
}			t_cmd;

void		close_pipe(int *pipe);
void		cmds_close_pipes(t_vec *cmds);
char		**cmd_str_to_argv(t_str_slice cmd, char *const *envp);

void		cmd_init(t_cmd *out);
void		cmd_del(void *cmd);
t_bool		cmds_pipe_io(t_cmd *from, t_cmd *to);
t_bool		cmd_new(const char *cmd_str, char *const *envp, t_cmd *out);
t_bool		cmd_new_with_file_in(const char *cmd_str, const char *file_in,
				char *const *envp, t_cmd *out);
t_bool		cmd_new_with_file_out(const char *cmd_str, const char *file_out,
				char *const *envp, t_cmd *out);
pid_t		cmd_exec(t_cmd *cmd, t_vec *cmds, char *const *envp);
int			cmds_exec_and_wait_all(t_vec *cmds, char *const *envp);

#endif
