#ifndef MATT_DAEMON_DAEMON_HPP
#define MATT_DAEMON_DAEMON_HPP

#include <fstream>
#include <iostream>

void		ft_crash(const std::string& message);

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
