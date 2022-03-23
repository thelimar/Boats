#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

int Captain (int sems_id, int n_seats, int n_rides);
int Passenger (int sems_id, int passenger_no, int n_seats);


int main (int argc, char** argv)
{
	printf ("God:  I CREATE THE BOAT\n");

	if (argc != 4)
	{
		printf ("Please enter 3 arguments: seats, passengers and rides\n");
		return 1;
	}

	int n_passangers = 0;
	int n_seats      = 0;
	int n_rides      = 0;

	sscanf (argv[1], "%d", &n_seats);
	sscanf (argv[2], "%d", &n_passangers);
	sscanf (argv[3], "%d", &n_rides);

	if (n_seats > n_passangers)
	{
		printf ("Passengers should be more than seats\n");
		return 2;
	}

	int sems_id = semget (IPC_PRIVATE, 3, 0666);
    unsigned short init_array[3] = {n_seats, 1, 1};
    semctl (sems_id, 0, SETALL, init_array);

    int captain_pid = fork ();

    if (captain_pid == 0)
    {
    	return Captain (sems_id, n_seats, n_rides);
    }

    for (int i = 1; i < n_passangers + 1; i++)
    {
    	int passenger_pid = fork ();

    	if (passenger_pid == 0)
    		return Passenger (sems_id, i, n_seats);
    }

	for (int i = 0; i < n_passangers + 1; i++)
		wait (NULL);


	printf ("GOD:  NOW I KILL THE BOAT\n");
	return 0;
}


int Captain (int sems_id, int n_seats, int n_rides)
{
	struct sembuf start_trip[2]     = {{0, 0, 1}, {2, 1, 0}};
	struct sembuf end_trip[2]       = {{0, -n_seats, 0}, {0, n_seats, 0}};
	struct sembuf start_boarding[1] = {{2, -1, 0}};

	printf ("Captain:  Alright, this is your captain speaking, hello everyone\n");

	for (int i = 0; i < n_rides; i++)
	{
		printf ("Captain: Welcome aboard everyone, our trip will start soon\n");

		semop (sems_id, start_boarding, 1);

		semop (sems_id, start_trip, 2);

		printf ("Captain: Ahoy!!!\n");

		semop (sems_id, end_trip, 2);

		printf ("Captain: Thank you for joining our trip. See you again!\n");
	}

	semctl (sems_id, 0, IPC_RMID);
	printf ("Captain:  Alright, my work for today is done...\n");

	return 0;
}


int Passenger (int sems_id, int passenger_no, int n_seats)
{
	struct sembuf board[4]   = {{0, -1, 0}, {1, -1, 0}, {2, 0, 0}, {1, 1, 0}};
	struct sembuf unboard[3] = {{0, 1, 0},  {1, -1, 0}, {1, 1, 0}};

	printf ("Passenger%d:  Yay! I am ready for voyage!\n", passenger_no);

	while (1)
	{
		if ((semop (sems_id, board, 4) == -1) && ((errno == EIDRM) || (errno == EINVAL)))
		{
			printf ("Passenger%d:  Too bad the ride are over :(. Bye-Bye!\n", passenger_no);
			return 0;
		}

		printf ("Passenger%d:  I am aboard, Hi everyone!\n", passenger_no);

		sleep (2);

		printf ("Passenger%d:  My-My, what a wonderful ride!\n", passenger_no);

		semop (sems_id, unboard, 3);

		printf ("Passenger%d:  Off to the ground again! I hope I can go one more time!\n", passenger_no);
	}
}