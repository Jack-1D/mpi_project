#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int myrank, nprocs ;
    
    MPI_Init( &argc, &argv ) ;

    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ) ;
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs ) ;
    int n = nprocs, k = 3;
    int count_index[2][n];
    for (int i = 0;i<n; i++)
        count_index[0][i] = i;
    while (1)
    {
        int kill_posi = count_index[0][(k - 1) % nprocs];
        if (myrank == kill_posi)
        {
            if(nprocs == 1)
                break;
            printf("Kill process%d\n", myrank);
            nprocs--;

            for (int i = 0; i < nprocs; i++)
                count_index[1][i] = count_index[0][(k+i)%(nprocs+1)];
            for (int i = 0; i < nprocs;i++)
                MPI_Send(&nprocs, 1, MPI_INT, count_index[1][i], 0, MPI_COMM_WORLD);
            for (int i = 0; i < nprocs; i++)
                count_index[0][i] = count_index[1][i];
            for (int i = 0; i < nprocs; i++)
                MPI_Send(&count_index, 8, MPI_INT, count_index[1][i], 0, MPI_COMM_WORLD);
            MPI_Finalize();
        }
        else{
            MPI_Recv(&nprocs, 1, MPI_INT, kill_posi, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(&count_index, 8, MPI_INT, kill_posi, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
    }
    if(myrank == count_index[0][0]){
        printf("Alive process is process%d\n", count_index[0][0]);
        MPI_Finalize();
    }
    return 0;
}