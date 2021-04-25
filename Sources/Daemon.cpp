#include <csignal>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "Daemon.hpp"
#include "Tintin_reporter.hpp"

using namespace std;

static const string lock_path = "matt_daemon.lock";
static const short	port = 4242;

static void ft_signal(int sig_no) {
    auto sig_name = "sig" + string(sys_signame[sig_no]);
    for (auto& c: sig_name)
        c = toupper(c);
	LOG("Catch a signal: " + sig_name);
}

Daemon::Daemon() {
	LOG("Matt_daemon started");
	for (int i = 0; i < NSIG; ++i)
        signal(i, ft_signal);
}

Daemon::~Daemon() {
	close(_lock_fd);
	if (_remove_lock)
		remove(lock_path.c_str());
	LOG("Matt_daemon is closed");
}

void Daemon::loop() {
	int					sock;
	struct sockaddr_in	addr, peer;
	char buff[1024];

	_lock_fd = open(lock_path.c_str(), O_CREAT, 0664);
	if (_lock_fd == -1)
		ft_crash("Can't open|create lock file at " + lock_path + ".\nError: " + strerror(errno));
	if (flock(_lock_fd, LOCK_EX | LOCK_NB) == -1)
		ft_crash("Lock file at " + lock_path + " is alredy locked");
	_remove_lock = true;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ft_crash("Can't create socket.\nError: " + string(strerror(errno)));
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (::bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
		ft_crash("Can't bind socket.\nError: " + string(strerror(errno)));
	if (listen(sock, SOMAXCONN) == -1)
		ft_crash("Can't listen to socket.\nError: " + string(strerror(errno)));
	socklen_t peer_addr_size = sizeof(struct sockaddr_in);
	if (accept(sock, (struct sockaddr *)&peer, &peer_addr_size) == -1)
		ft_crash("Accept failure.\nError: " + string(strerror(errno)));
	auto k = recv(sock, buff, 1024, 0);
	if (k > 0)
		cout << buff << endl;
}
