#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void createData(char* filename, int row_count, int column_count)
{
    int fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

    // Allocating memory for One block of data
    // Each block is comprised of 1024 tuples
    int tuples_per_block = 400;
    uint64_t tuple_size_in_bytes = column_count * sizeof(uint64_t);
    uint64_t block_size_in_bytes = tuple_size_in_bytes * tuples_per_block; 
    uint64_t *block_data = malloc(block_size_in_bytes);

    off_t file_offset = 0;
    uint64_t counter = 0;
    int rc = 0;
    for (int i = 0; i < row_count; i = i + tuples_per_block)
    {
        for (int b = 0; b < tuples_per_block; b++)
        {
            for (int j = 0; j < column_count; j++)
            {
                if (j == 0)
                    block_data[b * column_count] = rc * 10 + rand() % 10;
                else if (j == 1)
                {
                    if (i % 2 == 0)
                        block_data[b * column_count + 1] = (row_count - rc) * 10 - rand() % 10;
                    else
                        block_data[b * column_count + 1] = (row_count - rc) * 10 + rand() % 10;
                }
                else if (j == 2)
                    block_data[b * column_count + 2] = rand() % 1000;
                else if (j == 3)
                    block_data[b * column_count + 3] = rand() % 10000;
                else if (j == 4)
                    block_data[b * column_count + 4] = rand() % 100000;
                else
                    block_data[b * column_count + j] = counter++;
            }
            rc++;
        }
        
        int write_count = pwrite(fd, block_data, block_size_in_bytes, file_offset);
        if (write_count != block_size_in_bytes)
        {
            printf("SHOUT!!!!");
            break;
        }
        
        file_offset = file_offset + block_size_in_bytes;
    }
    free(block_data);
    close(fd);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    
    int row_count = atoi(argv[2]);
    int col_count = atoi(argv[3]);
    char *filename = argv[1];

    char* data_finename_with_extension;
    data_finename_with_extension = malloc(strlen(filename)+6);
    strcpy(data_finename_with_extension, filename);
    strcat(data_finename_with_extension, ".data"); 
    

    char* metadata_finename_with_extension;
    metadata_finename_with_extension = malloc(strlen(filename)+10);
    strcpy(metadata_finename_with_extension, filename); 
    strcat(metadata_finename_with_extension, ".metadata"); 
    

    clock_t start_t = clock();
    // data file
    createData(data_finename_with_extension, row_count, col_count);

    // MEtadata file
    FILE *fptr;
    fptr = fopen(metadata_finename_with_extension, "w");
    fprintf(fptr, "%s\n%d\n%d", filename, row_count, col_count);
    fclose(fptr);


    clock_t end_t = clock();
    float seconds_t = (float)(end_t - start_t) / CLOCKS_PER_SEC;

    printf("Filenmae %s, %s Row Count %d Column Count %d\n", data_finename_with_extension, metadata_finename_with_extension, row_count, col_count);
    printf("Time Taken to create data: %f \n", seconds_t);

    free(metadata_finename_with_extension);
    free(data_finename_with_extension);
    return 0;
}
