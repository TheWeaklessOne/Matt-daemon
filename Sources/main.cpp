#include <unistd.h>
#include <sys/stat.h>
#include <csetjmp>
#include <algorithm>
#include <csignal>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

void	ft_signal(int sig_no) {
	std::string sig_name = "sig" + std::string(sys_siglist[sig_no]);
	std::transform(sig_name.begin(), sig_name.end(), sig_name.begin(), ::toupper);

	LOG("Catch a signal: " + sig_name, SIGNAL);
	exit(EXIT_SUCCESS);
}

bool		is_file_exists(const std::string& path) {
	static struct stat buffer;

	return (stat(path.c_str(), &buffer) == 0);
}

void		ft_crash(const std::string& message) {
	LOG(message, ERROR);
	exit(1);
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

	if (is_file_exists(lock_path))
		std::cerr << "The program cannot be run in more than one instance!" << std::endl;

	daemonize();

	Daemon::instance().init_lock_file();
	Daemon::instance().loop();

	return (0);
}
