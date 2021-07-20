#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

static jmp_buf	env;

void		ft_crash() {
	longjmp(env, EXIT_FAILURE);
}

void		ft_crash(const std::string& message) {
	LOG(message, ERROR);
	longjmp(env, EXIT_FAILURE);
}

static void	daemonize() {
	pid_t pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	else if (pid > 0)
		exit(EXIT_SUCCESS);

	if (setsid() == -1)
		exit(EXIT_FAILURE);

	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	else if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);

	chdir("/");

	//	close(STDIN_FILENO);
	//	close(STDOUT_FILENO);
	//	close(STDERR_FILENO);
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
