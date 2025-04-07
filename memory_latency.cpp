// OS 2025 EX1

#include "memory_latency.h"
#include "measure.h"

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime(struct timespec t)
{
	return (uint64_t)(t.tv_sec) * 1000000000ull + (uint64_t)(t.tv_nsec);
}

/**
* Measures the average latency of accessing a given array in a sequential order.
* @param repeat - the number of times to repeat the measurement for and average on.
* @param arr - an allocated (not empty) array to preform measurement on.
* @param arr_size - the length of the array arr.
* @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
* @return struct measurement containing the measurement with the following fields:
*      double baseline - the average time (ns) taken to preform the measured operation without memory access.
*      double access_time - the average time (ns) taken to preform the measured operation with memory access.
*      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
*/
struct measurement measure_sequential_latency(uint64_t repeat, array_element_t* arr, uint64_t arr_size, uint64_t zero)
{
    // if too few repetition, we change repeat
    repeat = repeat < arr_size ? arr_size : repeat;

    // record the starting time
    struct timespec t0;
    timespec_get(&t0, TIME_UTC);
    register uint64_t rnd = 12345;  // Initialize with a starting value

    for (register uint64_t i = 0; i < repeat; i++){
        register uint64_t index = i % arr_size;  // Sequential access pattern
        rnd ^= index & zero;
        rnd = (rnd >> 1) ^ ((0-(rnd & 1)) & GALOIS_POLYNOMIAL);
    }
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);  // Record end time for baseline

    struct timespec t2;
    timespec_get(&t2, TIME_UTC);  // Record start time
    rnd = (rnd & zero) ^ 12345;
    for (register uint64_t i = 0; i < repeat; i++)
    {
        register uint64_t index = i % arr_size;
        rnd ^= arr[index] & zero;
        rnd = (rnd >> 1) ^ ((0-(rnd & 1)) & GALOIS_POLYNOMIAL);  // Advance rnd pseudo-randomly (using Galois LFSR)
    }

    struct timespec t3;
    timespec_get(&t3, TIME_UTC);  // Record end time for memory access

    // Calculate baseline and memory access times:
    double baseline_per_cycle=(double)(nanosectime(t1)- nanosectime(t0))/(repeat);
    double memory_per_cycle=(double)(nanosectime(t3)- nanosectime(t2))/(repeat);
    struct measurement result;

    result.baseline = baseline_per_cycle;
    result.access_time = memory_per_cycle;
    result.rnd = rnd;  // Return rnd to prevent compiler optimizations
    return result;
}

/**
 * Runs the logic of the memory_latency program. Measures the access latency for random and sequential memory access
 * patterns.
 * Usage: './memory_latency max_size factor repeat' where:
 *      - max_size - the maximum size in bytes of the array to measure access latency for.
 *      - factor - the factor in the geometric series representing the array sizes to check.
 *      - repeat - the number of times each measurement should be repeated for and averaged on.
 * The program will print output to stdout in the following format:
 *      mem_size_1,offset_1,offset_sequential_1
 *      mem_size_2,offset_2,offset_sequential_2
 *              ...
 *              ...
 *              ...
 */
int main(int argc, char* argv[])
{
    // zero==0, but the compiler doesn't know it. Use as the zero arg of measure_latency and measure_sequential_latency.
    struct timespec t_dummy;
    timespec_get(&t_dummy, TIME_UTC);
    const uint64_t zero = nanosectime(t_dummy)>1000000000ull?0:nanosectime(t_dummy);

    // Your code here
    if (argc != 4) {
        fprintf(stderr, "Usage: %s max_size factor repeat\n", argv[0]);
        return -1;
    }

    // Parse arguments
    uint64_t max_size = strtoull(argv[1], NULL, 10);
    double factor = strtod(argv[2], NULL);
    uint64_t repeat = strtoull(argv[3], NULL, 10);

    // Check for valid arguments
    if (max_size < 100){
        fprintf(stderr, "Error: max_size must be at least 100\n");
        return -1;
    }

    if (factor < 1.0){
        fprintf(stderr, "Error: factor must be greater than 1.0\n");
        return -1;
    }

    if (repeat == 0) {
        fprintf(stderr, "Error: repeat must be greater than 0\n");
        return -1;
    }

    // allocate arrays starting from 100 bytes
    uint64_t array_size_bytes = 100;

    // loop up to max_size using geometric series
    while(array_size_bytes <= max_size) {
      // find the number of elements
      uint64_t elements = array_size_bytes / sizeof(array_element_t);
      if (elements < 1) elements = 1;

      array_element_t* arr = (array_element_t*)malloc(elements * sizeof(array_element_t));
        if (arr == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return -1;
        }
        // Initialize the array with random values
        for (uint64_t i = 0; i < elements; i++) {
            arr[i] = i + 1;
        }

        struct measurement random_result = measure_latency(repeat, arr, elements, zero);
        struct measurement seq_result = measure_sequential_latency(repeat, arr, elements, zero);

        double random_offset = random_result.access_time - random_result.baseline;
        double seq_offset = seq_result.access_time - seq_result.baseline;

        printf("%lu,%.2f,%.2f\n", array_size_bytes, random_offset, seq_offset);

        free(arr);

        array_size_bytes = (uint64_t)(array_size_bytes * factor);

    }



}