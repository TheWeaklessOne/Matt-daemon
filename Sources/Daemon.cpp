#include <csignal>
#include <sys/file.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

static const std::string lock_path = "/Users/wstygg/Desktop/Matt-daemon/matt_daemon.lock";

static void	ft_signal(int sig_no) {
	std::string sig_name = "sig" + std::string(sys_signame[sig_no]);
	std::transform(sig_name.begin(), sig_name.end(), sig_name.begin(), ::toupper);

	LOG("Catch a signal: " + sig_name, SIGNAL);
	ft_crash();
}

Daemon::Daemon() {
	LOG("Matt_daemon started at PID: " + std::to_string(getpid()));

	for (int i = 0; i < NSIG; ++i)
		signal(i, ft_signal);
}

Daemon::~Daemon() {
	if (_lock_fd != -1)
		close(_lock_fd);

	if (_remove_lock) {
		flock(_lock_fd, LOCK_UN);

		if (remove(lock_path.c_str()) == 0)
			LOG("Lock file is removed");
		else
			LOG("Can't remove lock file", ERROR);
	}

	if (_remove_socket) {
		if (close(_socket) == 0)
			LOG("Socket is closed");
		else
			LOG("Can't close socket", ERROR);
	}
	LOG("Matt_daemon is closed");
}

void		Daemon::init_lock_file() {
	_lock_fd = open(lock_path.c_str(), O_CREAT, 0664);

	if (_lock_fd == -1)
		ft_crash("Can't open|create lock file at " + lock_path + ".\n\tError: " + strerror(errno));

	if (flock(_lock_fd, LOCK_EX | LOCK_NB) == -1)
		ft_crash("Lock file at " + lock_path + " is alredy locked");

	_remove_lock = true;
}

static void	start_connection(int fd) {
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
			LOG("Connection closed", USER_ACTION);
			break;
		}
	}
}

static int	listen_to_socket() {
	auto _socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in _sockaddr;
	_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(PORT);

	if (::bind(_socket, (sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1)
		ft_crash(std::string("Bind failure") + ".\n\tError: " + strerror(errno));

	if (listen(_socket, 3) == -1)
		ft_crash(std::string("Listen to socket error") + ".\n\tError: " + strerror(errno));

	return _socket;
}

void Daemon::loop() {
	sockaddr_in	addr;
	int			fd;

	_socket = listen_to_socket();
	_remove_lock = true;

	socklen_t lenght = sizeof(addr);
	while (true) {
		fd = accept(_socket, (sockaddr *)&addr, &lenght);

		if (fd == -1) {
			LOG("Accept returns bad fd");
			continue;
		}

		auto thread = std::thread(start_connection, fd);
		thread.detach();
	}
}
