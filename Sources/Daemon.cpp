#include <csignal>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

using namespace std;

static const string lock_path = "matt_daemon.lock";
static const short	port = 4242;

static void ft_signal(int sig_no) {
	auto sig_name = "sig" + string(sys_signame[sig_no]);
	for (auto& c: sig_name)
		c = toupper(c);
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
	if (_remove_lock)
		if (remove(lock_path.c_str()) == 0)
			LOG("Lock file is removed");
	LOG("Matt_daemon is closed");
}

void	Daemon::init_lock_file() {
	_lock_fd = open(lock_path.c_str(), O_CREAT, 0664);
	if (_lock_fd == -1)
		ft_crash("Can't open|create lock file at " + lock_path + ".\nError: " + strerror(errno));
	if (flock(_lock_fd, LOCK_EX | LOCK_NB) == -1)
		ft_crash("Lock file at " + lock_path + " is alredy locked");
	_remove_lock = true;
}

void Daemon::loop() {
	int sock = 0;
	struct sockaddr_in serv_addr;

	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		ft_crash();
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(4242);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		ft_crash();
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		cout << strerror(errno) << endl;
		ft_crash();
	}
	read(sock, buffer, 1024);
	printf("%s\n",buffer );
}
