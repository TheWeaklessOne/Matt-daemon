#include <algorithm>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

void	check_user_input(const std::string& input) {
	std::string capitalized = input;
	std::transform(capitalized.begin(), capitalized.end(), capitalized.begin(), ::toupper);

	auto first_word = std::string(input.substr(0, input.find(' ')));
	std::transform(first_word.begin(), first_word.end(), first_word.begin(), ::toupper);

	if (capitalized == "QUIT") {
		LOG("Exit by \"QUIT\" command", USER_ACTION);
		exit(EXIT_FAILURE);
	} else if (first_word == "SYSTEM") {
		std::string execute_string = input.substr(7);

		LOG(std::string("Got a \"System\" command, execute string:\n") + execute_string, USER_ACTION);
		
		auto pid = fork();

		signal(SIGCHLD, SIG_IGN);
		if (pid == 0) {
			Daemon::instance().set_is_children_process(true);
			system(execute_string.c_str());
			exit(0);
		}
		waitpid(pid, NULL, 0);
		signal(SIGCHLD, ft_signal);
	} else if (capitalized == "CLEAR") {
		Tintin_reporter::instance().clear_log();
		LOG("Log file was cleared", USER_ACTION);
	}
}
