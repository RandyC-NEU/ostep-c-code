#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// This is a very barebones vector impl to show realloc usage; supports
// - Pushing back
// - Modifying an element in place
// - Getting an element at index
// - Resizing quadratically (no user input)

struct IntVector
{
    int size;
    int capacity;
    int* data;
};
typedef struct IntVector IntVector;

int int_vector_construct(IntVector* pThis, int initial_capacity)
{
    pThis->size = 0;
    pThis->data = malloc(initial_capacity*sizeof(int));
    if (!pThis->data)
    {
        pThis->capacity = 0;
        return false;
    }
    else
    {
        pThis->capacity = initial_capacity;
        return true;
    }
}

//Quadratic scaling
bool int_vector_resize(IntVector* pThis)
{
    int new_capacity = pThis->capacity*2;
    int* new_data = realloc(pThis->data, new_capacity*sizeof(int));
    if (!new_data)
    {
        // Could not reallocate new memory, vector is still valid though.
        return false;
    }
    pThis->data = new_data;
    printf("Resize! [Old Capacity = %d New capacity = %d]\n", pThis->capacity, new_capacity);
    pThis->capacity = new_capacity;
    return true;
}

void int_vector_destroy(IntVector* pThis)
{
    pThis->size = 0;
    pThis->capacity = 0;
    free(pThis->data);
    pThis->data = NULL;
}

bool int_vector_push_back(IntVector* pThis, int elem)
{
    if (pThis->size >= pThis->capacity)
    {
        bool success = int_vector_resize(pThis);
        if (!success)
        {
            return success;
        }
    }
    pThis->data[pThis->size++] = elem;
    return true;
}

bool int_vector_at(IntVector* pThis, int idx, int* pOut)
{
    if (idx >= pThis->size)
    {
        return false;
    }
    if (pOut)
    {
        *pOut = pThis->data[idx];
    }
    return true;
}

bool int_vector_modify(IntVector* pThis, int idx, int new_val)
{
    if (idx >= pThis->size)
    {
        return false;
    }
    pThis->data[idx] = new_val;
    return true;
}

void int_vector_print(IntVector* pThis)
{
    printf("%s", "[");
    for (int i = 0; i < pThis->size; ++i)
    {
        printf("%d", pThis->data[i]);
        if (i < (pThis->size - 1))
        {
            printf("%s", ", ");
        }
    }
    printf("%s\n", "]");
}

#define MB (1024*1024)

void print_help(const char* exe_name)
{
    printf("Usage: %s <question_number [1, 4-8]>\n", exe_name);
}

void null_deref(void)
{
    int* ptr = NULL;
    int i = *ptr;
}

void alloc_but_no_free(void)
{
    unsigned long one_gib = 1024 * MB;
    int* arr = malloc(one_gib);
}

void realloc_big_arr(void)
{
    unsigned long one_gib = 1024 * MB;

    int* arr = malloc(5*sizeof(int));
    unsigned long count = one_gib / sizeof(int);
    while (1)
    {
        for (int i = 0; i < count; ++i)
        {
            arr[i] = i;
        }
    }
    free(arr);
}

void out_of_bounds_write(void)
{
    int* data = malloc(100 * sizeof(int));
    data[100] = 0;
    free(data);
}

void read_dealloc_mem(void)
{
    int* data = malloc(100*sizeof(int));
    data[0] = 100;
    free(data);
    printf("data[0] is %d\n", data[0]);
}

void free_ptr_not_from_alloc(void)
{
    int* data = malloc(5*sizeof(int));
    int* pInData = &data[1];
    free(pInData);
}

void use_vec(void)
{
    IntVector v = {0};
    if (!int_vector_construct(&v, 2))
    {
        fprintf(stderr, "%s\n", "Error during vector construction");
    }
    int_vector_push_back(&v, 1);
    int_vector_push_back(&v, 2);
    int_vector_print(&v);
    int_vector_push_back(&v, 3);
    int_vector_push_back(&v, 4);
    int_vector_push_back(&v, 5);
    int_vector_print(&v);
    int_vector_destroy(&v);
}

int main(int argc, char** argv)
{
    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        print_help(argv[0]);
    }
    else
    {
        char* endptr;
        unsigned long opt = strtoul(argv[1], &endptr, 10);
        if (strlen(endptr))
        {
            print_help(argv[0]);
            exit(0);
        }
        switch (opt)
        {
            case 1:
            {
                null_deref();
                break;
            }
            case 4:
            {
                alloc_but_no_free();
                break;
            }
            case 5:
            {
                out_of_bounds_write();
                break;
            }
            case 6:
            {
                read_dealloc_mem();
                break;
            }
            case 7:
            {
                free_ptr_not_from_alloc();
                break;
            }
            case 8:
            {
                use_vec();
                break;
            }
            default:
            {
                print_help(argv[0]);
                exit(1);
            }
        }
    }
}
