#include <stdio.h>
#include <stdlib.h>

#define MB (1024*1024)

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <num_mb_to_alloc>", argv[0]);
    }
    else
    {
        unsigned long num_mb = strtoul(argv[1], NULL, 10) * MB;
        unsigned long num_elem = num_mb / sizeof(int);
        int* arr = malloc(num_mb);
        while (1)
        {
            for (int i = 0; i < num_elem; ++i)
            {
                arr[i] = i;
            }
        }
    }
}