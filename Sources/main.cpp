#include <iostream>
#include <unistd.h>
#include <setjmp.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

using namespace std;

static jmp_buf	env;

void        ft_crash(const std::string &message) {
	LOG(message, true);
	longjmp(env, EXIT_FAILURE);
}

int	main() {
//	if (geteuid() != 0)
//		ft_crash("Program is have to run as root");

	pid_t pid = fork();
	if (pid < 0)
		ft_crash("Fork error");
	if (pid == 0) {
		Daemon daemon;
		int code = setjmp(env);
		if (code == 0) {
			daemon.loop();
		} else
			return (code);
	}
	exit(0);
}
