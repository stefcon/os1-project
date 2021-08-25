#include <iostream.h>
#include "list.h"
#include "system.h"
#include "userthr.h"



int main(int argc, char* argv[]) {

	System::initializeSystem();

	UserThread user_thread(argc, argv);
	user_thread.start();
	user_thread.waitToComplete();

	int return_value = user_thread.get_return_value();

	cout << "Return value of main thread is " << return_value << endl;

	System::restoreSystem();
	return return_value;

}

