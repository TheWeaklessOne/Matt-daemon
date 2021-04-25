#ifndef MATT_DAEMON_TINTIN_REPORTER_HPP
#define MATT_DAEMON_TINTIN_REPORTER_HPP

#include <fstream>
#include <iostream>

#define LOG Tintin_reporter::instance().log

void       ft_crash(const std::string& message);

class Tintin_reporter {
private:
	std::ofstream			_output;

	Tintin_reporter();

public:
    ~Tintin_reporter();
    Tintin_reporter(const Tintin_reporter& other) = delete;
    void operator=(const Tintin_reporter&) = delete;
	
	static Tintin_reporter&	instance();
	
	void					log(const std::string& message, bool is_error = false);
    
};

#endif
