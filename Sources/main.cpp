#include <unistd.h>
#include <sys/stat.h>
#include <csetjmp>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

static jmp_buf	env;

bool		is_file_exists(const std::string& path) {
	static struct stat buffer;

	return (stat(path.c_str(), &buffer) == 0);
}

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

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int	main() {
	if (geteuid() != 0) {
		std::cerr << "Program is have to be run as root!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (is_file_exists(lock_path)) {
		std::cerr << "The program cannot be run in more than one instance!" << std::endl;
		exit(EXIT_FAILURE);
	}

	daemonize();

	Daemon daemon;
	int code = setjmp(env);
	if (code == 0) {
		daemon.init_lock_file();
		daemon.loop();
	} else {
		return (code);
	}
}
