#include <csignal>
#include <sys/file.h>
#include <cerrno>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <cstring>
#include <sys/stat.h>
#include <algorithm>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

Daemon::Daemon() {
	LOG("Matt_daemon started at PID: [" + std::to_string(getpid()) + ']');

	for (int i = 0; i < NSIG; ++i)
		signal(i, ft_signal);
}

Daemon& Daemon::instance() {
	static Daemon daemon;

	return daemon;
}

Daemon::~Daemon() {
	if (_is_children_process)
		return;

	if (_lock_fd != -1)
		close(_lock_fd);

	if (_remove_lock) {
		flock(_lock_fd, LOCK_UN);

		if (remove(lock_path.c_str()) == 0)
			LOG("Lock file is removed");
		else
			LOG("Can't remove lock file", ERROR);
	}

	LOG("Matt_daemon at PID [" + std::to_string(getpid()) +  "] is closed");
}

void		Daemon::init_lock_file() {
	if (!is_file_exists(lock_dir))
		mkdir(lock_dir.c_str(), 0);

	_lock_fd = open(lock_path.c_str(), O_CREAT, 0664);

	if (_lock_fd == -1)
		ft_crash("Can't open|create lock file at " + lock_path + ".\n\tError: " + strerror(errno));

	if (flock(_lock_fd, LOCK_EX | LOCK_NB) == -1)
		ft_crash("Lock file at " + lock_path + " is already locked");

	_remove_lock = true;
}

static void	start_connection(int fd, int* _listeners_count) {
	LOG("New connection started", USER_ACTION);

	char buff[BUFFER_SIZE + 1];
	while (true) {
		bzero(&buff, BUFFER_SIZE);
		if (recv(fd, buff, BUFFER_SIZE, 0) > 0) {
			auto input = std::string(buff);

			input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
			LOG(input, USER_INPUT);

			check_user_input(input);
		}
		else {
			close(fd);
			--(*_listeners_count);

			LOG("Connection closed", USER_ACTION);
			break;
		}
	}
}

void	Daemon::listen_to_socket() {
	_socket = socket(AF_INET, SOCK_STREAM, 0);

	int enable = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		ft_crash("Setsockopt failure");

	sockaddr_in _sockaddr;
	_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(_port);

	if (::bind(_socket, (sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1)
		ft_crash(std::string("Bind failure") + ".\n\tError: " + strerror(errno));

	if (listen(_socket, MAX_LISTENERS_COUNT) == -1)
		ft_crash(std::string("Listen to socket error") + ".\n\tError: " + strerror(errno));
}

[[noreturn]] void Daemon::loop() {
	sockaddr_in	addr;
	int			fd;

	listen_to_socket();

	socklen_t lenght = sizeof(addr);
	while (true) {
		fd = accept(_socket, (sockaddr *)&addr, &lenght);

		if (_listeners_count < MAX_LISTENERS_COUNT) {
			if (fd == -1) {
				LOG("Accept returns bad fd");
				continue;
			}

			++_listeners_count;

			try {
				auto thread = std::thread(start_connection, fd, &_listeners_count);
				thread.detach();
			}
			catch (...) {
				LOG("Error on creating new connection", ERROR);
			}

		} else {
			LOG("User tried to connect when maximum user count was reached");
			close(fd);
		}
	}
}

void	Daemon::set_is_children_process(bool set) {
	_is_children_process = set;
}

void	Daemon::set_port(int set) {
	_port = set;
	LOG("Daemon port was set to " + std::to_string(set));
}
