#include "list.h"
#include "system.h"
#include "userthr.h"



int main(int argc, char* argv[]) {

	System::initializeSystem();

	UserThread user_thread(argc, argv);
	user_thread.start();
	user_thread.waitToComplete();

	int return_value = user_thread.get_return_value();
	System::restoreSystem();

	return return_value;

}

