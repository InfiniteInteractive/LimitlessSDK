
__kernel void rotateCopy(__constant float *src, int size, int pos, __global float *dst)
{
    const int index=get_global_id(0);

    int srcIndex=pos+index;

    if(srcIndex>size)
        srcIndex-=size;

    dst[index]=src[srcIndex];
}
