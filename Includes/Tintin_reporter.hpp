#ifndef MATT_DAEMON_TINTIN_REPORTER_HPP
#define MATT_DAEMON_TINTIN_REPORTER_HPP

#include <fstream>
#include <thread>
#include <mutex>

#define LOG Tintin_reporter::instance().log
#define MAX_LOG_SIZE 1024

void		ft_crash();
void		ft_crash(const std::string& message);
bool		is_file_exists(const std::string& message);

#define FOREACH_MESSAGE_TYPE(TYPE)	\
	TYPE(INFO)						\
	TYPE(SIGNAL)					\
	TYPE(ERROR)						\
	TYPE(USER_INPUT)				\
	TYPE(USER_ACTION)				\
	TYPE(MESSAGE_TYPES_N)			\

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum message_type {
	FOREACH_MESSAGE_TYPE(GENERATE_ENUM)
};

static const char *message_type_names[MESSAGE_TYPES_N + 1] = {
	FOREACH_MESSAGE_TYPE(GENERATE_STRING)
};

static const std::string log_dir = "/var/log/matt_daemon/";
static const std::string log_path = log_dir + "matt_daemon.log";

class Tintin_reporter {
private:
	std::ofstream	_output;
	time_t			_time = std::time(nullptr);
	std::mutex		_file_mutex;
	std::mutex		_write_mutex;

	Tintin_reporter();
	~Tintin_reporter();

	void	create_log_file();

public:
	Tintin_reporter(const Tintin_reporter& other) = delete;
	void operator=(const Tintin_reporter&) = delete;
	
	static Tintin_reporter&	instance();
	
	void	log(const std::string& message, message_type type = INFO);
	void	clear_log();
};

#endif
