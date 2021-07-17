#include <iostream>
#include <unistd.h>
#include <setjmp.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

static jmp_buf	env;

void		ft_crash() {
	longjmp(env, EXIT_FAILURE);
}

void        ft_crash(const std::string& message) {
	LOG(message, ERROR);
	longjmp(env, EXIT_FAILURE);
}

void		exit(const std::string& message) {
	std::cerr << message << std::endl;
	longjmp(env, EXIT_FAILURE);
}

static void	daemonize() {
	pid_t pid = fork();

	if (pid < 0)
		exit("Daemonize failed! (fork error)");
	else if (pid > 0)
		exit(EXIT_SUCCESS);
}

int	main() {
//	if (geteuid() != 0)
//		exit("Program is have to be run as root!");

	daemonize();

	Daemon daemon;
	int code = setjmp(env);
	if (code == 0) {
		daemon.init_lock_file();
		daemon.loop();
	} else
		return (code);

	return (EXIT_SUCCESS);
}
