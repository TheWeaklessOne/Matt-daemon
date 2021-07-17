#include <csignal>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

static const std::string lock_path = "matt_daemon.lock";

static void	ft_signal(int sig_no) {
	std::string sig_name = "sig" + std::string(sys_signame[sig_no]);
	std::transform(sig_name.begin(), sig_name.end(), sig_name.begin(), ::toupper);

	LOG("Catch a signal: " + sig_name, SIGNAL);
	ft_crash();
}

Daemon::Daemon() {
	LOG("Matt_daemon started");
	for (int i = 0; i < NSIG; ++i)
		signal(i, ft_signal);
}

Daemon::~Daemon() {
	if (_lock_fd != -1)
		close(_lock_fd);

	if (_remove_lock) {
		if (remove(lock_path.c_str()) == 0)
			LOG("Lock file is removed");
		else
			LOG("Can't remove lock file", ERROR);
	}

	LOG("Matt_daemon is closed");
}

void		Daemon::init_lock_file() {
	_lock_fd = open(lock_path.c_str(), O_CREAT, 0664);

	if (_lock_fd == -1)
		ft_crash("Can't open|create lock file at " + lock_path + ".\nError: " + strerror(errno));

	if (flock(_lock_fd, LOCK_EX | LOCK_NB) == -1)
		ft_crash("Lock file at " + lock_path + " is alredy locked");

	_remove_lock = true;
}

static void	check_user_input(const std::string& input) {
	std::string capitalized = input;
	std::transform(capitalized.begin(), capitalized.end(), capitalized.begin(), ::toupper);

	if (capitalized == "quit")
		exit("Exit by quit command");
}

static void	start_thread(int fd, void* thread_address) {
	const auto thread_id = std::this_thread::get_id();

	LOG("Thread started");

	char buff[BUFFER_SIZE + 1];
	while (true)
	{
		bzero(&buff, BUFFER_SIZE);
		if (recv(fd, buff, BUFFER_SIZE, 0) > 0)
		{
			auto input = std::string(buff);
			//			content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
			LOG(input, USER_INPUT, thread_id);

			check_user_input(input);
		}
		else
		{
			LOG("Client disconect");
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
		ft_crash(std::string("Bind failure") + ".\nError: " + strerror(errno));

	if (listen(_socket, 3) == -1)
		ft_crash(std::string("Listen to socket error") + ".\nError: " + strerror(errno));

	return _socket;
}

void Daemon::loop() {
	auto		_socket = listen_to_socket();
	sockaddr_in	addr;
	int			fd;

	socklen_t lenght = sizeof(addr);
	while (true)
	{
		fd = accept(_socket, (sockaddr *)&addr, &lenght);

		if (fd == -1) {
			LOG("Accept returns bad fd");
			continue;
		}

		auto thread = std::thread(start_thread, fd);
		thread.detach();
	}
}
