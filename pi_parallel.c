/**
 * Parallel Pi Computation Program
 *
 * Computes π using numerical integration with OpenMP parallelization
 * Method: ∫(4.0/(1+x²))dx from 0 to 1 = π
 * Approximation: Sum of rectangles under the curve
 *
 * Best practices implemented:
 * - Reduction clause (most scalable, eliminates false sharing)
 * - Parallel for worksharing construct
 * - Proper private variables
 * - Timing instrumentation
 */

#include <stdio.h>
#include <string>
#include <omp.h>

#define NUM_STEPS 100000000   // Number of intervals (100 million)
#define NUM_THREADS 4         // Default number of threads

int main(int argc, char* argv[])
{
    long i;                    // Loop counter
    double x;                  // Midpoint of interval i
    double pi;                 // Computed value of π
    double sum = 0.0;          // Accumulated sum (shared, will be reduced)
    double step;               // Width of each rectangle
    double start_time, end_time;  // Timing variables

    // Calculate step size
    step = 1.0 / (double)NUM_STEPS;

    // Optional: Allow user to specify number of threads via command line
    int num_threads = NUM_THREADS;
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    }

    // Set number of threads for OpenMP
    omp_set_num_threads(num_threads);

    printf("Computing π with %ld steps using %d threads...\n",
        NUM_STEPS, num_threads);
    printf("Method: Numerical integration (rectangle method)\n\n");

    // Start timing
    start_time = omp_get_wtime();

    /**
     * PARALLEL REGION WITH REDUCTION
     *
     * Key OpenMP features used:
     *
     * 1. #pragma omp parallel for
     *    - Creates a team of threads (parallel)
     *    - Divides loop iterations among threads (worksharing for)
     *    - Combined construct for simplicity
     *
     * 2. private(x)
     *    - Each thread gets its own copy of 'x'
     *    - Prevents race conditions on x variable
     *    - Thread-local variable initialized independently
     *
     * 3. reduction(+:sum)
     *    - Each thread maintains a local private copy of 'sum'
     *    - Threads update their local copy with '+=' operations
     *    - After the parallel region, OpenMP combines all local
     *      copies using the '+' operator
     *    - This is the most efficient method:
     *      ✓ No false sharing (unlike array approach)
     *      ✓ No critical sections (unlike critical approach)
     *      ✓ Automatic and scalable
     *
     * 4. Loop index 'i' is automatically private by default
     */
#pragma omp parallel for private(x) reduction(+:sum)
    for (i = 0; i < NUM_STEPS; i++) {
        // Midpoint of interval i (x = i+0.5)*step
        x = (i + 0.5) * step;

        // Function value: 4.0/(1.0 + x*x)
        // Each thread accumulates into its local copy of 'sum'
        sum += 4.0 / (1.0 + x * x);
    }

    // Multiply by step width to get final π value
    pi = step * sum;

    // End timing
    end_time = omp_get_wtime();

    // Print results
    printf("========================================\n");
    printf("Computed π = %.15f\n", pi);
    printf("Actual π   = %.15f\n", 3.141592653589793);
    printf("Error      = %.15f\n", pi - 3.141592653589793);
    printf("----------------------------------------\n");
    printf("Execution time = %.4f seconds\n", end_time - start_time);
    printf("========================================\n");

    return 0;
}
