// Matrix operations

__kernel void add(__global float *a, __global float *b, __global float *c)
{
    int ind=get_global_id(0);
    c[ind]=a[ind]+b[ind];
}

__kernel void diff(__global float *a, __global float *b, __global float *c)
{
    int ind=get_global_id(0);
    c[ind]=a[ind]-b[ind];
}

#define ref(a,x,y, width) ((a)[(x)*(width)+(y)])

__kernel void mlt(__global float *a, __global float *b, __global float *c, int rh1, int rh2)
{
    int x=get_global_id(0), y=get_global_id(1);
    ref(c,x,y,rh1)=0.0f;
    for (int i=0;i<rh2;i++)
    {
        ref(c,x,y,rh1)+=ref(a,x,i,rh1)*ref(b,i,y,rh2);
    }
}
