#include "Tintin_reporter.hpp"

#include <sys/stat.h>

static const std::string log_path = "/Users/daniilteterin/Desktop/daemon/matt_daemon.log";

Tintin_reporter::Tintin_reporter() {
	create_log_file();
}

void	Tintin_reporter::create_log_file() {
	_file_mutex.lock();

	if (_output.is_open())
		_output.close();

	_output.exceptions(std::fstream::failbit | std::fstream::badbit);

	try {
		_output.open(log_path.c_str(), std::fstream::in | std::fstream::app);
	}
	catch (const std::fstream::failure& e) {
		ft_crash();
	}

	_file_mutex.unlock();
}

Tintin_reporter::~Tintin_reporter() {
	if (_output.is_open())
		_output.close();
}

Tintin_reporter& Tintin_reporter::instance() {
	static Tintin_reporter reporter;
	
	return reporter;
}

static bool		is_log_exists() {
	static struct stat buffer;

	return (stat(log_path.c_str(), &buffer) == 0);
}

void	Tintin_reporter::log(const std::string& message, message_type type) {
	_write_mutex.lock();

	const auto tm = *std::localtime(&_time);

	if (!is_log_exists()) {
		create_log_file();
		LOG("Seems like log_file was deleted, recreate");
	}

	try {
		_output << std::put_time(&tm, "[%d/%m/%Y-%H:%M:%S] ");

		_output << '[' << message_type_names[type] << "]\t";

		if (type == USER_INPUT || type == USER_ACTION) {
			_output << '[' << std::this_thread::get_id() << "] ";
		}

		_output << message << std::endl;
	}
	catch (const std::ofstream::failure& e) {
		ft_crash();
	}

	_write_mutex.unlock();
}

void	Tintin_reporter::clear_log() {
	_file_mutex.lock();

	if (_output.is_open())
		_output.close();

	_output.exceptions(std::fstream::failbit | std::fstream::badbit);

	try {
		_output.open(log_path.c_str(), std::fstream::in | std::fstream::trunc);
	}
	catch (const std::fstream::failure& e) {
		ft_crash();
	}

	_file_mutex.unlock();
}
