#include "Tintin_reporter.hpp"

using namespace std;

Tintin_reporter::Tintin_reporter() {
    _output.exceptions(fstream::failbit | fstream::badbit);
    const string path = "matt_daemon.log";
    try {
        _output.open(path, fstream::in | fstream::app);
    }
    catch (const fstream::failure& e) {
        ft_crash("Can't create log file at " + path + "\n" + "Exception: " + e.what());
    }
}

Tintin_reporter::~Tintin_reporter() {
    _output.close();
}

Tintin_reporter& Tintin_reporter::instance() {
	static Tintin_reporter reporter;
	
	return reporter;
}

void	Tintin_reporter::log(const std::string& message, bool is_error) {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

    try {
		_output << std::put_time(&tm, "[%d/%m/%Y-%H:%M:%S] ");
		if (is_error)
			_output << "[ERROR] ";
        _output << message << endl;
		cout << message << endl;
    }
    catch (const std::ofstream::failure &e) {
		ft_crash("Write to log error\nException: " + string(e.what()));
    }
}
