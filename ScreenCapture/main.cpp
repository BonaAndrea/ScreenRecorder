//#include <bits/stdc++.h>
#include "ScreenRecorder.h"

using namespace std;

/* driver function to run the application */
int main()
{
	ScreenRecorder screen_record;

	screen_record.openVideoDevice();
	screen_record.openAudioDevice();
	screen_record.initOutputFile();
	screen_record.captureAudio();
	screen_record.captureVideoFrames();

	cout << "\nprogram executed successfully\n";

	return 0;
}
