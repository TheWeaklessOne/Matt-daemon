#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
	const std::string path = "matt_daemon.log";

	_output.exceptions(std::fstream::failbit | std::fstream::badbit);
	
	try {
		_output.open(path, std::fstream::in | std::fstream::app);
	}
	catch (const std::fstream::failure& e) {
		ft_crash();
	}
}

Tintin_reporter::~Tintin_reporter() {
	_output.close();
}

Tintin_reporter& Tintin_reporter::instance() {
	static Tintin_reporter reporter;
	
	return reporter;
}

void	Tintin_reporter::log(const std::string& message, message_type type, std::__thread_id thread_id) {
	const auto tm = *std::localtime(&_time);

	try {
		_output << std::put_time(&tm, "[%d/%m/%Y-%H:%M:%S] ");

		_output << '[' << message_type_names[type] << "] ";

		if (type == USER_INPUT) {
			_output << '[' << thread_id << "] ";
		}

		_output << message << std::endl;
	}
	catch (const std::ofstream::failure& e) {
		ft_crash();
	}
}
