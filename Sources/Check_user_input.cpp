#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

void check_user_input(const std::string& input) {
	std::string capitalized = input;
	std::transform(capitalized.begin(), capitalized.end(), capitalized.begin(), ::toupper);

	auto first_word = std::string(input.substr(0, input.find(' ')));
	std::transform(first_word.begin(), first_word.end(), first_word.begin(), ::toupper);

	if (capitalized == "QUIT") {
		LOG("Exit by \"QUIT\" command", USER_ACTION);
		ft_crash();
	} else if (first_word == "SYSTEM") {
		std::string execute_string = input.substr(7);

		LOG(std::string("Got a \"System\" command, execute string:\n") + execute_string, USER_ACTION);
		system(execute_string.c_str());
	} else if (capitalized == "CLEAR") {
		Tintin_reporter::instance().clear_log();
		LOG("Log file was cleared", USER_ACTION);
	}
}
