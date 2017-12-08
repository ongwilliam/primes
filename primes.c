// Assignment 4 for CS 3376.501
// Programmer: William Ong
// NetID: wco150030
// Description:
// primes.c contains the main function and helper functions to handle the calculation of primes in range of 2-1,000,000
//		with the help of 10 child processes

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>

#define READ 0
#define WRITE 1

// Mergesort C implementation (method1) from http://geeksquiz.com/merge-sort/
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;

	/* create temp arrays */
	int L[n1], R[n2];

	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	/* Copy the remaining elements of L[], if there
	are any */
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	/* Copy the remaining elements of R[], if there
	are any */
	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

// Mergesort C implementation (method2) from http://geeksquiz.com/merge-sort/
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
	if (l < r)
	{
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l + (r - l) / 2;

		// Sort first and second halves
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);

		merge(arr, l, m, r);
	}
}

// Checks if number is prime using trial division
int isPrime(unsigned int n)
{
	unsigned int p;
	// First prime number is 2
	if (!(n & 1) || n < 2) return n == 2;

	/* comparing p*p <= n can overflow */
	for (p = 3; p <= n / p; p += 2)
	{
		// If factorable it is NOT prime
		if (!(n % p))
		{
			return 0;
		}
	}
	return 1;
}

// Return primes array for 2 - 1000 (168 total primes)
int * lessThanThousand()
{
	int * lessThanThousand;
	int arrCtr = 0;
	
	// Allocate memory
	lessThanThousand = (int *)malloc(sizeof(int)*168);

	// Check each number between 2-1000 for primality
	int i;
	for (i = 2; i <= 1000; i++) 
	{
		// If prime, add number to array and update array index
		if (isPrime(i))
		{
			lessThanThousand[arrCtr] = i;
			arrCtr++;
		}
	}
	return lessThanThousand;
}

// Checks if number is prime using 2-1000 primes array (168 total primes)
int simpleCheck(unsigned int n)
{
	int * simpleCheck = lessThanThousand();

	int i;
	for (i = 0; i < 168; i++)
	{
		// If factorable by a prime it is NOT prime
		if (!(n % simpleCheck[i]))
		{
			return 0;
		}
	}
	return 1;
}

// Child process prime calculation
//		Start->end is range of prime calculation
//      In and out are read and write pipes
void childCalculate(int start, int end, int in, int out)
{
	// Close child read pipe
	close(in);

	// File locking - Set to non-blocking
	fcntl(out, F_SETFL, O_NONBLOCK);

	// Check each number for primality
	int i;
	for (i = start; i <= end; i++)
	{
		// If prime, add number to array and update array index
		if (simpleCheck(i))
		{
			// Write prime to parent
			int writePrime = i;
			write(out, &writePrime, sizeof(writePrime));
		}
	}

}

// MAIN FUNCTION - Create 10 concurrent child processes; calculate and pipe primes to parent process. 
//		Then sort and output primes from the parent process
int main(void)
{
	pid_t pids[10];
	int i;
	int n = 10;

	int pipefd[2];

	// Handle pipe creation errors
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	// Propagate children and calculate primes
	for (i = 0; i < n; ++i)
	{
		// Handle fork creation errors
		if ((pids[i] = fork()) < 0)
		{
			perror("fork");
			abort();
		}

		// (Inside child processes)
		else if (pids[i] == 0)
		{
			// Assign ranges for each of the 10 child processes
			switch (i)
			{
				case 0: // 1,001-100,000
				{
					childCalculate(1001, 100000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 1: // 100,001-200,000
				{
					childCalculate(100001, 200000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 2: // 200,001-300,000
				{
					childCalculate(200001, 300000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 3: // 300,001-400,000
				{
					childCalculate(300001, 400000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 4: // 400,001-500,000
				{
					childCalculate(400001, 500000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 5: // 500,001-600,000
				{
					childCalculate(500001, 600000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 6: // 600,001-700,000
				{
					childCalculate(600001, 700000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 7: // 700,001-800,000
				{
					childCalculate(700001, 800000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 8: // 800,001-900,000
				{
					childCalculate(800001, 900000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				case 9: // 900,001-1,000,0000
				{
					childCalculate(900001, 1000000, pipefd[READ], pipefd[WRITE]);
					break;
				}
				default:
				{
					break;
				}

			}

			// Close child write pipe
			close(pipefd[WRITE]);
			exit(0);
		}

	}

	int * thousand = lessThanThousand();
	int * primes;
	int arrCtr = 0;
	// Allocate memory for <= 1,000,000 primes
	primes = (int *)malloc(sizeof(int) * 78498);
	
	// Add <= 1,000 primes to array
	for (i = 0; i < 168; i++)
	{
		primes[arrCtr] = thousand[i];
		arrCtr++;
	}

	// Read primes and wait for children to exit
	int status;
	pid_t pid;
	while (n > 0) {
		// Close parent write pipe
		close(pipefd[WRITE]);

		// Reading primes that were written from children
		int writePrime;
		int x;
		while ((x = read(pipefd[READ], &writePrime, sizeof(int))) == sizeof(int))
		{
			primes[arrCtr] = writePrime;
			arrCtr++;
		}

		// Wait on children to exit
		pid = wait(&status);
		--n; 
	}

	// Close parent read pipe
	close(pipefd[READ]);

	// Parameters to sort array: 
	//		mergeSort(array , leftIndex , rightIndex);
	mergeSort(primes, 0, 78497);

	// Print total primes array (78498 total primes)
	for (i = 0; i < 78498; i++)
	{
		printf("Prime #%d: %d\n", i+1, primes[i]);
	}

	return 0;
}