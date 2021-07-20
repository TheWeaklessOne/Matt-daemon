#ifndef MATT_DAEMON_DAEMON_HPP
#define MATT_DAEMON_DAEMON_HPP

#include <fstream>
#include <iostream>

#define PORT 4242
#define BUFFER_SIZE 128
#define MAX_LISTENERS_COUNT 3

void		ft_crash(const std::string& message);
void		check_user_input(const std::string& input);

class Daemon {
private:
	int		_lock_fd;
	bool	_remove_lock = false;
	bool	_remove_socket = false;
	int		_socket;
	int		_listeners_count = 0;

	void	check_user_input();
	
public:
	Daemon();
	~Daemon();
	Daemon(const Daemon& other) = delete;
	void operator=(const Daemon&) = delete;

	void	loop();
	void	init_lock_file();
};

#endif
