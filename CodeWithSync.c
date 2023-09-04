#include <stdio.h>
#include <mpi.h>

int count_index[2][5];

int find_index(int target, int np){
    for (int i = 0; i < np;i++){
        if(count_index[1][i] == target)
            return i;
    }
}

int main(int argc, char* argv[]) {
    int myrank, nprocs ;
    
    MPI_Init( &argc, &argv ) ;

    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ) ;
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs ) ;
    int n = nprocs, k = 3;
    
    for (int i = 0;i<n; i++)
        count_index[0][i] = i;

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group pre_group;
    MPI_Group_incl(world_group, nprocs, count_index[0], &pre_group);
    MPI_Comm pre_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, pre_group, 0, &pre_comm);
    MPI_Group post_group;
    MPI_Comm post_comm;
    int killed_index_prime[1] = {k - 1};

    while (1)
    {
        if(nprocs == 1)
            break;

        int kill_posi = count_index[0][(k - 1) % nprocs];

        if(nprocs < 4){
            pre_group = post_group;
            pre_comm = post_comm;

            kill_posi = count_index[0][(k - 1) % nprocs];
            killed_index_prime[0] = find_index(kill_posi, nprocs);
            killed_index_prime[0] = killed_index_prime[0] == 0 ? nprocs - 1 : killed_index_prime[0] - 1;
        }
        MPI_Barrier(pre_comm);
        
        MPI_Comm_group(pre_comm, &post_group);
        MPI_Group_excl(post_group, 1, &killed_index_prime[0], &post_group);
        MPI_Comm_create_group(pre_comm, post_group, 0, &post_comm);
        if (myrank == kill_posi)
        {
            
            printf("Kill process%d\n", myrank);
            nprocs--;

            for (int i = 0; i < nprocs; i++)
                count_index[1][i] = count_index[0][(k+i)%(nprocs+1)];
            for (int i = 0; i < nprocs; i++)
                count_index[0][i] = count_index[1][i];
            for (int i = 0; i < nprocs + 1; i++)
            {
                if (i == killed_index_prime[0])
                    continue;
                MPI_Send(&nprocs, 1, MPI_INT, i, 0, pre_comm);
            }
            for (int i = 0; i < nprocs + 1; i++){
                if(i == killed_index_prime[0])
                    continue;
                MPI_Send(&count_index, 8, MPI_INT, i, 0, pre_comm);
            }
            
        }
        else{
            int row_rank, row_size;
            MPI_Recv(&nprocs, 1, MPI_INT, killed_index_prime[0], 0, pre_comm, MPI_STATUS_IGNORE);
            MPI_Recv(&count_index, 8, MPI_INT, killed_index_prime[0], 0, pre_comm,MPI_STATUS_IGNORE);
        }
        
        MPI_Barrier(pre_comm);
        if(myrank == kill_posi){
            MPI_Finalize();
        }
    }
    
    if(myrank == count_index[0][0]){
        printf("Alive process is process%d\n", count_index[0][0]);
        MPI_Finalize();
    }
    
    return 0;
} 