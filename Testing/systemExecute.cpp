#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#ifdef OS_WINDOWS
   void remove()
   {
   	system("del temp.ts");
   }
#else
  void remove()
   {
   	system("rm temp.ts");
   }
#endif

int main(void)
{
	system("ffmpeg -i image.jpg -loglevel 0 -vcodec mpeg2video -f mpegts temp.ts");

	remove();
	return 0;
}
