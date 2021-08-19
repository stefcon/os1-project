#ifndef USERTHR_H_
#define USERTHR_H_

#include <thread.h>

int userMain (int argc, char* argv[]);

class UserThread : public Thread {
public:
	UserThread(int argc, char* argv[]) : Thread(), argc_(argc), argv_(argv), return_value_(0) {}
	~UserThread() { waitToComplete(); };
	int get_return_value() const { return return_value_; }

	virtual Thread* clone() const { return new UserThread(argc_, argv_);}
protected:
	void run() { return_value_ = userMain(argc_, argv_); }
private:
	int argc_, return_value_;
	char** argv_;
};



#endif /* USERTHR_H_ */
