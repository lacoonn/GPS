#include <stdio.h>
#include <Windows.h>

int main()
{
	HANDLE hComm;
	wchar_t str[1000] = L"COM7";

	hComm = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
		printf("Error in opening serial port\n");
	else
		printf("opening serial port successful\n");

	DCB dcbSerialParams = { 0 }; // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	bool Status = GetCommState(hComm, &dcbSerialParams);

	dcbSerialParams.BaudRate = CBR_9600;  // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;  // Setting Parity = None

	SetCommState(hComm, &dcbSerialParams);

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50; // in milliseconds
	timeouts.ReadTotalTimeoutConstant = 50; // in milliseconds
	timeouts.ReadTotalTimeoutMultiplier = 10; // in milliseconds
	timeouts.WriteTotalTimeoutConstant = 50; // in milliseconds
	timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds

	/*
	//
	//Writing Data to Serial Port
	//
	char lpBuffer[] = "A";
	DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
	DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port
	dNoOFBytestoWrite = sizeof(lpBuffer);

	Status = WriteFile(hComm,        // Handle to the Serial port
		lpBuffer,     // Data to be written to the port
		dNoOFBytestoWrite,  //No of bytes to write
		&dNoOfBytesWritten, //Bytes written
		NULL);
	*/

	//
	// Reading from the Serial Port
	//
	char TempChar; //Temporary character used for reading
	char SerialBuffer[256]; //Buffer for storing Rxed Data
	DWORD NoBytesRead;
	int i = 0;

	do
	{
		ReadFile(hComm,           //Handle of the Serial port
			&TempChar,       //Temporary character
			sizeof(TempChar),//Size of TempChar
			&NoBytesRead,    //Number of bytes read
			NULL);

		/*
		SerialBuffer[i] = TempChar;// Store Tempchar into buffer
		if (TempChar == '\n') {
			SerialBuffer[i + 1] = 0;
			puts(SerialBuffer);
			i = 0;
		}
		else
			i++;
		*/
		if (NoBytesRead > 0) {

		putchar(TempChar);

		}
	}
	while (1);

	// Close HANDLE Comm
	CloseHandle(hComm);
}