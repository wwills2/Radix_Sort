/*
 * James Weber
 * CMSC 483
 * 5/1/2023
 *
 * compile: mpicc parallel_quick_sort.c -o pqs
 * run: mpirun -np <processes> ./pqs [array size] [number of digits in int] [check for correct sorting <0/1>] [print unsorted and sorted lists <0/1>] 
 *   The parameters are optional and the defaults for the options are, a list size of 10 times the number of processes,
 *      5 digit numbers, do check for correct sorting of the list, and don't print the lists to the console.
 * Ex: mpirun -np 6 ./pqs
 *     mpirun -np 3 ./pqs 100
 *     mpirun -np 3 ./pqs 100 5
 *     mpirun -np 6 ./pqs 40 5 1 1
 *     mpirun -np 4 ./pqs 100000 5 0 1
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "timer.c"

// Function definitions
void swap(int* arr, int i, int j);
void quicksort(int* arr, int start, int end);
int* merge(int* arr1, int n1, int* arr2, int n2);

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) ((id+1)*(n)/(p) - 1)
#define BLOCK_SIZE(id,p,n) ((id+1)*(n)/(p) - (id)*(n)/(p))
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))


int main(int argc, char* argv[])
{
    int n;
    int* list = NULL;
    int element_size, own_element_size, print_list, verify, num_digits;
    int* element;

    int np, my_pid;
    int rc = MPI_Init(&argc, &argv);
    MPI_Status status;

    if (rc != MPI_SUCCESS) {
        printf("Error in creating MPI program.\n Terminating......\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_pid);

    if (argc < 2) {
        n = np * 10;
    } else {
        if (argc > 1) {
            n = strtol(argv[1], NULL, 10);
            if (n < np) {
                if (my_pid == 0)
                    printf("The number of list items has to be at least as large as the number of processes.\n");
                MPI_Finalize();
                return 0;
            }
            if (n % np != 0) {
                if (my_pid == 0)
                    printf("The number of list items has to be evenly divisible by the number of processes.\n");
                MPI_Finalize();
                return 0;
            }
        }
        if (argc > 2) {
            num_digits = strtol(argv[2], NULL, 10);
        } else {
            num_digits = 5;
        }
        if (argc > 3) {
            verify = strtol(argv[3], NULL, 10);
        } else {
            verify = 1;
        }
        if (argc > 4) {
            print_list = strtol(argv[4], NULL, 10);
        } else {
            print_list = 0;
        }
    }

    double start, stop, elapsed;

    if (my_pid == 0) {

        GET_TIME(start)

        list = (int*)malloc(n * sizeof(int));
        
        srand(time(NULL));
        int max = 1;
        for (int i = 0; i < num_digits; i++) {
            max *= 10;
        }
        int min = max / 10;
        max -= min;
        for (int i = 0; i < n; i++) {
            list[i] = rand() % max + min;
        }

        printf("Parallel Quicksort on a list of %d ints.\n", n);
        // Printing the array, if requested
        if (print_list) {
            printf("Unsorted array: ");
            for (int i = 0; i < n; i++) {
                printf("%d ", list[i]);
            }
            printf("\n");
        }

    }

    // Blocks all process until reach this point
    MPI_Barrier(MPI_COMM_WORLD);

    // Stop the timer
    //todo s_time_taken += MPI_Wtime();

    // Starts Timer
    //todop_time_taken -= MPI_Wtime();

    // BroadCast the Size to all the process from root process
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Computing element size
    element_size = (n % np == 0) ? (n / np) : n / (np - 1);

    // Calculating total size of element according to bits
    element = (int*)malloc(element_size * sizeof(int));

    // Scatter the chuck size list to all process
    MPI_Scatter(list, element_size, MPI_INT, element, element_size, MPI_INT, 0, MPI_COMM_WORLD);
    free(list);
    list = NULL;

    // Compute size of own element and then sort them using quick sort
    own_element_size = (n >= element_size * (my_pid + 1)) ? element_size : (n - element_size * my_pid);
    
    quicksort(element, 0, own_element_size);

    for (int step = 1; step < np; step = 2 * step) {
        if (my_pid % (2 * step) != 0) {
            MPI_Send(element, own_element_size, MPI_INT, my_pid - step, 0, MPI_COMM_WORLD);
            break;
        }
        if (my_pid + step < np) {
            int received_element_size = (n >= element_size * (my_pid + 2 * step)) ? (element_size * step)
                      : (n - element_size * (my_pid + step));
            int* element_received;
            element_received = (int*)malloc(received_element_size * sizeof(int));
            MPI_Recv(element_received, received_element_size, MPI_INT, my_pid + step, 0, MPI_COMM_WORLD, &status);

            list = merge(element, own_element_size, element_received, received_element_size);
            int total = own_element_size + received_element_size;

            free(element);
            free(element_received);
            element = list;
            own_element_size = own_element_size + received_element_size;
        }
    }

    if (my_pid == 0){
        GET_TIME(stop)
        elapsed = stop - start;
        printf("RUNTIME: %f\n\n", elapsed);
    }

    // Printing the results
    if (my_pid == 0) {
        if (verify) {
            int i, check_sort = 1;
            if (print_list) {
                printf("  Sorted array: ");
                for (i = 0; i < n - 1; i++) {
                    if (element[i] > element[i + 1])
                        check_sort = 0;
                    printf("%d ", element[i]);
                }
                printf("%d\n", element[i]);
            } else {
                for (i = 0; i < n - 1; i++)
                    if (element[i] > element[i + 1])
                        check_sort = 0;
            }

            if (check_sort) {
                printf("Sorting %d ints Performed Correctly.\n", i + 1);
            } else {
                printf("Sorting %d ints Failed.\n", i + 1);
            }
        } else {
            printf("Sorting Operation Complete.\n");
        }
    }

    MPI_Finalize();
    return 0;
}

// Function to swap two numbers
void swap(int* arr, int i, int j)
{
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}

// Function that performs the Quick Sort for an array arr[] starting from the
//   index start and ending at index end
void quicksort(int* arr, int start, int end)
{
    int pivot;
    int index;

    // Base Case
    if (end <= 1)
        return;

    // Pick pivot and swap with first element Pivot is middle element
    pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);

    // Partitioning Steps
    index = start;

    // Iterate over the range [start, end]
    for (int i = start + 1; i < start + end; i++) {

        // Swap if the element is less than the pivot element
        if (arr[i] < pivot) {
            index++;
            swap(arr, i, index);
        }
    }

    // Swap the pivot into place
    swap(arr, start, index);

    // Recursive Call for sorting of quick sort function
    quicksort(arr, start, index - start);
    quicksort(arr, index + 1, start + end - index - 1);
}

// Function that merges the two arrays
int* merge(int* arr1, int n1, int* arr2, int n2)
{
    int* result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;

    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2) {
            result[k] = arr1[i];
            i++;
        }

        // Indices in bounds as i < n1 && j < n2
        else if (arr1[i] < arr2[j]) {
            result[k] = arr1[i];
            i++;
        }

            // v2[j] <= v1[i]
        else {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}
