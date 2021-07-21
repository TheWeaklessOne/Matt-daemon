#ifndef MATT_DAEMON_DAEMON_HPP
#define MATT_DAEMON_DAEMON_HPP

#include <fstream>
#include <iostream>

#define PORT 4242
#define BUFFER_SIZE 128
#define MAX_LISTENERS_COUNT 3

void		ft_crash(const std::string& message);
void		check_user_input(const std::string& input);
bool		is_file_exists(const std::string& message);

static const std::string	lock_dir = "/var/lock/";
static const std::string	lock_path = lock_dir + "matt_daemon.lock";

class Daemon {
private:
	int		_lock_fd = 0;
	bool	_remove_lock = false;
	int		_socket = 0;
	int		_listeners_count = 0;
	
public:
	Daemon();
	~Daemon();
	Daemon(const Daemon& other) = delete;
	void operator=(const Daemon&) = delete;

	[[noreturn]] void	loop();
	void	init_lock_file();
};

#endif
