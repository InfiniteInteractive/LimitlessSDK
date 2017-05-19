__constant sampler_t nearestSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

kernel void yuv422torgb(global uchar4 *src, uint srcWidth, uint srcHeight, write_only image2d_t dst, uint dstWidth, uint dstHeight)
{
    //	int2 coord = (int2)(get_global_id(0), get_global_id(1));
    uint gx=get_global_id(0);
    uint gy=get_global_id(1);

    uint srcX=gx;
    uint dstX=gx*2;
    uint y=gy;

//	write_imageui(dst, (int2) { dstX, y }, (uint4){255, 0, 0, 255});
//    write_imageui(dst, (int2) { dstX+1, y }, (uint4) { 255, 0, 0, 255 });
//	return;

    if((dstX < dstWidth) && (y < dstHeight))
    {
        uchar4 values=src[y*(srcWidth/2)+srcX];

//        write_imageui(dst, (int2) { dstX, y }, (uint4){ values[1], values[1], values[1], 255});
//        write_imageui(dst, (int2) { dstX+1, y }, (uint4) { values[3], values[3], values[3], 255 });
//		return;

        float Cr=values[2]/255.0f-0.5f;
        float Cb=values[0]/255.0f-0.5f;
        float4 p;
        float Y;

        Y=1.1643*(values[1]/255.0f-0.0625);

        p.s0=Y+2.017*Cr;
        p.s1=Y-0.39173*Cr-0.81290*Cb;
        p.s2=Y+1.5958*Cb;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { dstX, y }, convert_uint4(p));

        Y=1.1643*(values[3]/255.0f-0.0625);

        p.s0=Y+2.017*Cr;
        p.s1=Y-0.39173*Cr-0.81290*Cb;
        p.s2=Y+1.5958*Cb;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { dstX+1, y }, convert_uint4(p));
    }
}

kernel void yuv422ptorgb(global uchar *src, uint srcWidth, uint srcHeight, write_only image2d_t dst, uint dstWidth, uint dstHeight)
{
    //	int2 coord = (int2)(get_global_id(0), get_global_id(1));
    uint gx=get_global_id(0);
    uint gy=get_global_id(1);

    uint x=gx*4;
    uint y=gy;

    if((x < dstWidth)&&(y < dstHeight))
    {
    	float4 p;
    
    	float Y = 1.1643 * (src[x + y*srcWidth] / 255.0f - 0.0625);
    	float Cr = src[gx+(y*(srcWidth/2))+srcWidth*srcHeight] / 255.0f - 0.5f;
    	float Cb = src[gx+(y*(srcWidth/2))+((srcWidth/2)*srcHeight)+(srcWidth)*(srcHeight/2)] / 255.0f - 0.5f;
    
    	p.s0 = Y + 1.5958 * Cb;
    	p.s1 = Y - 0.39173*Cr-0.81290*Cb;
    	p.s2 = Y + 2.017*Cr;
    	p.s3 = 1.0f;
//    	write_imagef(dst, (int2){ x, y }, p);
        write_imageui(dst, (int2) { x, y }, convert_uint4(p));
    
    	Y = 1.1643 * (src[x + 1 + y*srcWidth] / 255.0f - 0.0625);
    	p.s0 = Y + 1.5958 * Cb;
    	p.s1 = Y - 0.39173*Cr-0.81290*Cb;
    	p.s2 = Y + 2.017*Cr;
    	p.s3 = 1.0f;
//    	write_imagef(dst, (int2){ x+1, y }, p);
        write_imageui(dst, (int2) { x+1, y }, convert_uint4(p));
    }
    //    write_imageui(dst, (int2) { x, y }, (uint4){255, 0, 0, 255});
    //    write_imageui(dst, (int2) { x+1, y }, (uint4) { 255, 0, 0, 255 });
}

__kernel void yuv420ptorgb(global uchar *src, uint srcWidth, uint srcHeight, __write_only image2d_t dst, uint dstWidth, uint dstHeight)
{
    uint gx=get_global_id(0);
    uint gy=get_global_id(1);

    uint x=gx*2;
    uint y=gy*2;

    if((x<dstWidth)&&(y<dstHeight))
    {
        float4 p;
        uint pos=srcWidth*srcHeight;
        uint halfWidth=(srcWidth/2);
        uint halfHeight=(srcHeight/2);
        float4 Y;
        
        Y[0]=src[srcWidth*y+x];
        Y[1]=src[srcWidth*y+x+1];
        Y[2]=src[srcWidth*(y+1)+x];
        Y[3]=src[srcWidth*(y+1)+x+1];

        Y=1.1643*(Y/255.0f-0.0625);
        float Cr=((float)src[pos+(gy*halfWidth)+gx])/255.0f-0.5f;
        pos+=halfWidth*halfHeight;
        float Cb=((float)src[pos+(gy*halfWidth)+gx])/255.0f-0.5f;

        p.s0=Y[0]+1.5958 * Cb;
        p.s1=Y[0]-0.39173*Cr-0.81290*Cb;
        p.s2=Y[0]+2.017*Cr;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { x, y }, convert_uint4(p));

        p.s0=Y[1]+1.5958 * Cb;
        p.s1=Y[1]-0.39173*Cr-0.81290*Cb;
        p.s2=Y[1]+2.017*Cr;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { x+1, y }, convert_uint4(p));

        p.s0=Y[2]+1.5958 * Cb;
        p.s1=Y[2]-0.39173*Cr-0.81290*Cb;
        p.s2=Y[2]+2.017*Cr;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { x, y+1 }, convert_uint4(p));

        p.s0=Y[3]+1.5958 * Cb;
        p.s1=Y[3]-0.39173*Cr-0.81290*Cb;
        p.s2=Y[3]+2.017*Cr;
        p.s3=1.0f;
        p*=255.0f;

        write_imageui(dst, (int2) { x+1, y+1 }, convert_uint4(p));
    }

//    uchar Y1=src[srcWidth*y+x];
//    uchar Y2=src[srcWidth*y+x+1];
//    uchar Y3=src[srcWidth*(y+1)+x];
//    uchar Y4=src[srcWidth*(y+1)+x+1];
//
//    write_imageui(dst, (int2) { x, y }, (uint4) { Y1, Y1, Y1, 255 });
//    write_imageui(dst, (int2) { x+1, y }, (uint4) { Y2, Y2, Y2, 255 });
//    write_imageui(dst, (int2) { x, y+1 }, (uint4) { Y3, Y3, Y3, 255 });
//    write_imageui(dst, (int2) { x+1, y+1 }, (uint4) { Y4, Y4, Y4, 255 });
}

__kernel void yuyvtorgb(global uchar *src, uint srcWidth, uint srcHeight, __write_only image2d_t dst, uint dstWidth, uint dstHeight)
{
	uint gx = get_global_id(0);
	uint x = gx*2;
	uint gy = get_global_id(1);

	if ((x+1 < dstWidth) & (gy<dstHeight))
	{
		uint off=(gy*srcWidth*2)+(x*2);
		float Cb = (255/112)*0.886*(src[off]-128);
		float Y0 = (255/219)*(src[off+1]-16);
		float Cr = (255/112)*0.701*(src[off+2]-128);
		float Y1 = (255/219)*(src[off+3]-16);
		float4 p;

//		if(gy == 1079)
//		{
//		p.s0 = 255;
//		p.s1 = 0;
//		p.s2 = 0;
//		write_imageui(dst, (int2){ x, gy }, convert_uint4(p));
//
//		p.s0 = 255;
//		p.s1 = 0;
//		p.s2 = 0;
//		write_imageui(dst, (int2){ x+1, gy }, convert_uint4(p));
//		}
//		else
		{
		p.s0 = Y0 + Cr;
		p.s1 = Y0 - (0.114/0.587)*Cb-(0.299/0.587)*Cr;
		p.s2 = Y0 + Cb;
		p.s3 = 1.0f;
		write_imageui(dst, (int2){ x, gy }, convert_uint4(p));

		p.s0 = Y1 + Cr;
		p.s1 = Y1 - (0.114/0.587)*Cb-(0.299/0.587)*Cr;
		p.s2 = Y1 + Cb;
		write_imageui(dst, (int2){ x+1, gy }, convert_uint4(p));
		}
	}
}


__kernel void rgbtoyuyv(__read_only image2d_t src, uint width, uint height, global uchar *dst)
{
	uint gx = get_global_id(0);
	uint x = gx*2;
	uint gy = get_global_id(1);

	if ((x+1 < width) & (gy<height))
	{
		uint off=(gy*width*2)+(x*2);

		uint4 value0=read_imageui(src, nearestSampler, (int2){x, gy});
		uint4 value1=read_imageui(src, nearestSampler, (int2){x+1, gy});

		uint Y0=16+(65.738/256)*value0.x+(129.057/256)*value0.y+(25.064/256)*value0.z;
		uint Cb0=128-(37.945/256)*value0.x-(74.494/256)*value0.y+(112.439/256)*value0.z;
		uint Cr0=128+(112.439/256)*value0.x-(94.154/256)*value0.y-(18.285/256)*value0.z;

		uint Y1=16+(65.738/256)*value1.x+(129.057/256)*value1.y+(25.064/256)*value1.z;
		uint Cb1=128-(37.945/256)*value1.x-(74.494/256)*value1.y+(112.439/256)*value1.z;
		uint Cr1=128+(112.439/256)*value1.x-(94.154/256)*value1.y-(18.285/256)*value1.z;

		dst[off]=(Cb0+Cb1)/2.0;
		dst[off+1]=Y0;
		dst[off+2]=(Cr0+Cr1)/2.0;
		dst[off+3]=Y1;
	}
}