#include <iostream>
#include <spawn.h>

#include "Daemon.hpp"

#define KEYWORD "daemon"

using namespace std;

void	ft_crash(const string &message) {
	cerr << message << endl;
	exit(1);
}

int	main(int argc, char *argv[], char *envp[]) {
	pid_t pid;

	if (argc > 1 && strcmp(argv[1], "daemon") == 0) {
		Daemon matt;
		matt.loop();
	} else {
		char **new_argv = (char **) malloc(sizeof(char *) * argc + 2);
		if (!new_argv)
			ft_crash("new_argv malloc error");
		new_argv[0] = strdup(argv[0]);
		new_argv[1] = strdup(KEYWORD);
		memcpy(new_argv + 2, argv + 1, sizeof(char *) * argc - 1);
		new_argv[argc + 1] = nullptr;
		if (posix_spawn(&pid, argv[0], nullptr, nullptr, new_argv, envp) != 0)
			ft_crash("posix spawn error");
	}
	exit(0);
}
