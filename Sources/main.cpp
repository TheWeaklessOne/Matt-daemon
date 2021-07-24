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

int	main(int argc, char *argv[]) {
	if (geteuid() != 0) {
		std::cerr << "Program is have to be run as root!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (is_file_exists(lock_path))
		std::cerr << "The program cannot be run in more than one instance!" << std::endl;

	int	opt = 0;
	int	port = PORT;
	while ((opt = getopt(argc, argv, ":p:")) != - 1) {
		switch(opt) {
			case 'p':
				try {
					port = std::stoi(optarg);;

					if (port <= 0 || port >= 0xFFFF) {
						std::cerr << "port number is have to be between 1 and 65536" << std::endl;
						exit(EXIT_FAILURE);
					}
				}
				catch (...) {
					std::cerr << "Can't convert to number - \"" << optarg << '"' << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			case ':':
				std::cerr << "option -p is have to be followed by parameter" << std::endl;
				exit(EXIT_FAILURE);
		}
	}

	daemonize();

	Daemon::instance().init_lock_file();
	Daemon::instance().set_port(port);
	Daemon::instance().loop();

	return (0);
}
