/**
 * ============================================================================
 * MPI SEND AND RECEIVE EXAMPLE - Ping Pong Communication
 * ============================================================================
 * 
 * PROBLEM: Demonstrate point-to-point communication between MPI processes
 * 
 * DESCRIPTION:
 * - Process 0 sends a message to Process 1
 * - Process 1 receives the message and sends a reply back
 * - Process 0 receives the reply
 * 
 * COMPILATION:
 *     mpicc mpi_pingpong.c -o mpi_pingpong
 * 
 * EXECUTION:
 *     mpirun -np 2 ./mpi_pingpong
 * 
 * AUTHOR: Based on standard MPI tutorial patterns
 * ============================================================================
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank;           /* Process rank (ID) - 0, 1, 2, ... */
    int size;           /* Total number of processes */
    int tag = 42;       /* Message tag (used to identify messages) */
    
    int send_data;      /* Data to send */
    int recv_data;      /* Data received */
    
    MPI_Status status;  /* Status object for receive operation */
    
    /* ========== INITIALIZE MPI ========== */
    /* Must be called before any other MPI functions */
    MPI_Init(&argc, &argv);
    
    /* Get the rank (ID) of this process */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    /* Get the total number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    /* ========== CHECK FOR CORRECT NUMBER OF PROCESSES ========== */
    if (size != 2) {
        if (rank == 0) {
            printf("ERROR: This program requires exactly 2 processes!\n");
            printf("Run with: mpirun -np 2 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    /* ========== PROCESS 0: SEND AND RECEIVE ========== */
    if (rank == 0) {
        /* Prepare data to send */
        send_data = 100;
        
        printf("Process %d: Sending data (%d) to Process 1...\n", rank, send_data);
        
        /* SEND: Send data to process 1 */
        /* Parameters: &data, count, datatype, destination, tag, communicator */
        MPI_Send(&send_data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        
        printf("Process %d: Waiting to receive reply from Process 1...\n", rank);
        
        /* RECEIVE: Receive reply from process 1 */
        /* Parameters: &buffer, count, datatype, source, tag, communicator, status */
        MPI_Recv(&recv_data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
        
        printf("Process %d: Received reply (%d) from Process 1\n", rank, recv_data);
        printf("\n=== PING PONG COMPLETE ===\n");
    }
    
    /* ========== PROCESS 1: RECEIVE AND SEND BACK ========== */
    else if (rank == 1) {
        printf("Process %d: Waiting to receive data from Process 0...\n", rank);
        
        /* RECEIVE: Receive data from process 0 */
        MPI_Recv(&recv_data, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        
        printf("Process %d: Received data (%d) from Process 0\n", rank, recv_data);
        
        /* Modify the data (optional) */
        send_data = recv_data * 2;
        
        printf("Process %d: Sending reply (%d) back to Process 0...\n", rank, send_data);
        
        /* SEND: Send reply back to process 0 */
        MPI_Send(&send_data, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        
        printf("Process %d: Reply sent!\n", rank);
    }
    
    /* ========== CLEAN UP AND EXIT ========== */
    MPI_Finalize();
    
    return 0;
}
