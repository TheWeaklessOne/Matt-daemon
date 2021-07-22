#include <sys/stat.h>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
	create_log_file();
}

void	Tintin_reporter::create_log_file() {
	_file_mutex.lock();

	if (_output.is_open())
		_output.close();

	if (!is_file_exists(log_dir))
		if (mkdir(log_dir.c_str(), 0) == -1)
			exit(EXIT_FAILURE);

	_output.exceptions(std::fstream::failbit | std::fstream::badbit);

	try {
		_output.open(log_path.c_str(), std::fstream::in | std::fstream::app);
	}
	catch (...) {
		exit(EXIT_FAILURE);
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

static std::string	get_compressed_filename() {
	static time_t	_time = std::time(nullptr);
	const auto tm = *std::localtime(&_time);
	std::stringstream stream;

	stream << std::put_time(&tm, "%d.%m.%Y-%H:%M");
	return stream.str() + ".log" + ".zip";
}

static bool	compress_log() {
	auto size = std::filesystem::file_size(log_path);

	if (size > MAX_LOG_SIZE) {
		const auto compressed_filename = get_compressed_filename();
		const auto system_string = "zip " + log_dir + compressed_filename + " " + log_path;
		
		auto pid = fork();

		signal(SIGCHLD, SIG_IGN);
		if (pid == 0) {
			Daemon::instance().set_is_children_process(true);
			system(system_string.c_str());
			exit(0);
		}
		waitpid(pid, NULL, 0);
		signal(SIGCHLD, ft_signal);
		return true;
	}
	return false;
}

void	Tintin_reporter::log(const std::string& message, message_type type) {
	_write_mutex.lock();

	const auto tm = *std::localtime(&_time);

	if (!is_file_exists(log_path)) {
		create_log_file();
		_write_mutex.unlock();
		LOG("Seems like log_file was deleted, recreate");
		_write_mutex.lock();
	}

	if (compress_log()) {
		clear_log();
		_write_mutex.unlock();
		LOG("File size was reached maximum size and was compressed as " + get_compressed_filename());
		_write_mutex.lock();
	}

	try {
		_output << std::put_time(&tm, "[%d/%m/%Y-%H:%M:%S] ");

		_output << '[' << message_type_names[type] << "]\t";

		if (type == USER_INPUT || type == USER_ACTION) {
			_output << "[ID: " << std::this_thread::get_id() << "] ";
		}

		_output << message << std::endl;
	}
	catch (const std::ofstream::failure& e) {
		exit(EXIT_FAILURE);
	}

	_write_mutex.unlock();
}

void	Tintin_reporter::clear_log() {
	_file_mutex.lock();

	if (_output.is_open())
		_output.close();

	if (!is_file_exists(log_dir))
		if (mkdir(log_dir.c_str(), 0) == -1)
			exit(EXIT_FAILURE);

	_output.exceptions(std::fstream::failbit | std::fstream::badbit);

	try {
		_output.open(log_path.c_str(), std::fstream::in | std::fstream::trunc);
	}
	catch (...) {
		exit(EXIT_FAILURE);
	}

	_file_mutex.unlock();
}
