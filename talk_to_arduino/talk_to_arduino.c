#include <stdio.h>   // printf(), scanf()
#include <string.h>  // memset(), strlen()
#include <fcntl.h>	 // open()
#include <errno.h>	 // strerror()
#include <termios.h> // POSIX terminal control definitions
#include <unistd.h>	 // write(), read(), close()

void menu()
{
	// function of display menu on screen
	char str[10][70] =
		{"%% WELCOME TO GTU ARDUINO LAB        %%\n",
		 "%% STUDENT NAME: FATMA ÖZTÜRK        %%\n",
		 "%% PLEASE SELECT FROM THE FOLLOWING  %%\n",
		 "%% MENU :                            %%\n",
		 "(1) TURN ON LED ON ARDUINO\n",
		 "(2) TURN OFF LED ON ARDUINO\n",
		 "(3) FLASH ARDUINO LED 5 TIMES\n",
		 "(4) SEND A NUMBER TO ARDUINO TO COMPUTE FACTORIAL BY ARDUINO\n",
		 "(0) EXIT\n",
		 "PLEASE SELECT: "};

	for (int i = 0; i < 10; ++i)
	{
		printf("%s", str[i]);
	}
	fflush(stdout); // flush printf's buffer
}

int init_serial_port()
{
	int serial_port = open("/dev/tty.usbmodem112301", O_RDWR | O_NONBLOCK);

	// Create new termios struct
	struct termios tty;

	// Read in existing settings, and handle any error
	if (tcgetattr(serial_port, &tty) != 0)
	{
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		return (-1);
	}

	tty.c_cflag &= ~PARENB;		   // Clear parity bit, disabling parity
	tty.c_cflag &= ~CSTOPB;		   // Clear stop field, only one stop bit used in communication
	tty.c_cflag &= ~CSIZE;		   // Clear all bits that set the data size
	tty.c_cflag |= CS8;			   // 8 bits per byte
	tty.c_cflag &= ~CRTSCTS;	   // Disable RTS/CTS hardware flow control
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL); // Disables canonical mode, ECHO, and others.
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_cc[VTIME] = 0; // Sets inter-character timeout to 0.
	tty.c_cc[VMIN] = 0; // Sets the minimum number of characters to 0.

	// Set in/out baud rate to be 9600, same arduino's baud rate
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	// Save tty settings, also checking for error
	tcsetattr(serial_port, TCSANOW, &tty);
	if (tcsetattr(serial_port, TCSAFLUSH, &tty) != 0)
	{
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return (-1);
	}

	sleep(2); // required to make flush work
	tcflush(serial_port, TCIOFLUSH);
	return (serial_port);
}

void write_to_serial_port2(int fd_number, char second_number)
{
	// When number 4 selected from menu
	// Write to serial port
	char msg[256];
	memset(msg, 0, sizeof(msg)); // clear msg
	msg[0] = second_number;
	msg[1] = '\n';
	int len = strlen(msg);
	size_t sent = write(fd_number, msg, len);
}

void write_to_serial_port(int fd_number, char number)
{
	// When number 1, 2, 3 selected from menu
	char msg[256];
	memset(msg, 0, sizeof(msg));
	// reassign 1, 2, 3 to avoid confusion
	if (number == '1')
	{
		msg[0] = 'a';
		msg[1] = '\n';
	}
	else if (number == '2')
	{
		msg[0] = 'b';
		msg[1] = '\n';
	}
	else if (number == '3')
	{
		msg[0] = 'c';
		msg[1] = '\n';
	}
	int len = strlen(msg);
	size_t sent = write(fd_number, msg, len);
}

int read_from_serial_port(int fd_num)
{
	const int buf_max = 256;
	int timeout = 5000;
	char read_buf[1];
	char buf[buf_max];
	int i = 0;
	while (read_buf[0] != '\n' && i < buf_max && timeout > 0)
	{
		// read character one by one
		int n = read(fd_num, read_buf, 1);
		if (n == -1)
			return -1; // couldn't read
		if (n == 0)
		{
			usleep(1000); // wait 1 msec try again
			timeout--;
			if (timeout == 0)
				return -2;
			continue;
		}
		buf[i] = read_buf[0];
		i++;
	}
	buf[i] = 0;
	printf("Arduino's message: %s\n", buf);
	return (0);
}

int main()
{
	int fd;
	menu();
	fd = init_serial_port();
	char selected_number;
	char second_number = '\0';
	while (scanf(" %c", &selected_number) == 1)
	{
		if (selected_number == '1' || selected_number == '2' || selected_number == '3')
		{
			write_to_serial_port(fd, selected_number);
			read_from_serial_port(fd);
		}
		else if (selected_number == '4')
		{
			printf("Enter a number to calculate factorial : ");
			scanf(" %c", &second_number);
			write_to_serial_port2(fd, second_number);
			read_from_serial_port(fd);
		}
		else if (selected_number == '0')
		{
			close(fd);
			break;
		}
		else
		{
			printf("Invalid input, please enter a valid number: ");
		}
	}
	return (0);
};