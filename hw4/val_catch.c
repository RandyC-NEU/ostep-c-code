#include <stdio.h>

int* ret_stack_ptr(void)
{
    int ret = 3;
    return &ret;
}

int main(void)
{
    int* pRet = ret_stack_ptr();
    printf("Val of ret is: %d\n", *pRet);
    return 0;
}