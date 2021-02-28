#include <iostream>
#include <csignal>
#include <fstream>

#include "Daemon.hpp"

using namespace std;

static void	ft_signal(int sig_no) {
	static ofstream out("output.txt", ios_base::app);

	switch (sig_no) {
		case SIGHUP:
			out << "SIGHUP!" << endl;
			break;
		case SIGINT:
			out << "SIGINT!" << endl;
			break;
		case SIGSEGV:
			out << "SIGSEGV!" << endl;
			break;
		default:
			out << "Jopa..." << endl;
			exit(2);
	}
}

Daemon::Daemon() {
	cout << "Я родился!" << std::endl;
	for (int i = 0; i < NSIG; ++i)
		signal(i, ft_signal);
}


void Daemon::loop() {
	while (1)
		;
}
