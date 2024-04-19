/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwurster <kwurster@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 16:36:07 by kwurster          #+#    #+#             */
/*   Updated: 2024/04/19 19:10:10 by kwurster         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// open, close, read, write, malloc, free
// access, dup, dup2, unlink (delete file)
// perror, strerror
// execve, fork, wait, waitpid, pipe

// fork duplicates the current process and returns the pid of the child process or -1 to the parent process, while the duplicated process gets 0
// execve transforms the current process into a new one with new args, this function only returns if an error occurs
// waitpid waits for a specific child process to terminate, if the child process has already terminated, it returns immediately. It gives access to the exit status of the child process
// pipe creates a pipe, which is a unidirectional data channel that can be used for interprocess communication
// dup2 duplicates a file descriptor, replacing the old one with the new one (replace stdin with pipe read end, replace stdout with pipe write end)

// create pipes
// fork process
// parent opens file and writes to pipe
// child reads from pipe and
// pipe file via get_next_line to forked process
//
