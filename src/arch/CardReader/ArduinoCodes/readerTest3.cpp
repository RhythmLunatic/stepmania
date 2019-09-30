#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include <fcntl.h>

/*template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}*/

using namespace std;
int main()
{
	//cout << "Test" << endl;
	//fstream tty;
	int fd;
	fd = open("/dev/ttyUSB0", O_RDONLY);
	if (fd == -1)
	{
		cout << "ERROR!!!";
		return -1;
	}
	//tty.open("test", ios::in);
	
	char * buffer = new char [11];
	//cout << "reading..." << endl;
	while (true)
	{
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);

		struct timeval zero = {0,0};
		if ( select(fd+1, &fdset, NULL, NULL, &zero) <= 0 )
			continue;
		read(fd, buffer, 11);
		
		
		
		/*if ((int)buffer[0] != 0)
		{
			cout << "No card." << std::endl;
			//return 0;
		}*/
		stringstream hexStr;
		for (int i = 2; i < 11; i++)
		{
			int b = (int)buffer[i];
			if (b < 0)
			b = 256+b;
			hexStr << std::hex << b;
		}
		
		/*for (int i = 0; i < 10; i++)
		{
			printf("%i:",(int)buffer[i]);
		}
		cout << std::endl;*/
		
		std::time_t t = std::time(0);  // t is an integer type
		cout << "[" << t << "] " << hexStr.str() << std::endl;
		

		//usleep(5000);
	}
	
	//cout << buffer;
	
	
	//array can be filled with "FFFFFFFF" (8 chars)
	/*char hex_string[8];
	for (int i = 2; i < 10; i++)
	{
		int b = (int)buffer[i];
		if (b < 0)
		b = 256+b;
		printf("%02X:",b);
		//printf("%i:",(int)buffer[i]);
		//sprintf(hex_string, "%X",(int)buffer[i]);
		//cout << hex_string;
		//cout << (int)buffer[i] << ":";
	}
	cout << std::endl;*/
	

	
	/*for (int i = 2; i < 10; i++)
	{
		printf("%i:",(int)buffer[i]);
	}
	cout << std::endl;
	int testArray[8] = {4,88,22,-110,-96,59,-124,0};
	for (int i = 0; i < 8; i++)
	{
		printf("%X:",(signed int)testArray[i]);
	}*/
	
	
	//Exit program
	//tty.close();
	return 0;
}
