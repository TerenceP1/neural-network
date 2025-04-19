// Matrix operations

__kernel void add(__global float *a, __global float *b, __global float *c)
{
    int ind=get_global_id(0);
    c[ind]=a[ind]+b[ind];
}
