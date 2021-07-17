#ifndef MATT_DAEMON_DAEMON_HPP
#define MATT_DAEMON_DAEMON_HPP

#include <fstream>
#include <iostream>

#define PORT 4242
#define BUFFER_SIZE 128

void		ft_crash(const std::string& message);
void		exit(const std::string& message);

class Daemon {
private:
	int		_lock_fd;
	bool	_remove_lock = false;
	
public:
	Daemon();
	~Daemon();

	void	loop();
	void	init_lock_file();
};

#endif
