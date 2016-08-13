__constant sampler_t nearestSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;
__constant sampler_t linearSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_LINEAR;

kernel void overlay(read_only image2d_t src, read_only image2d_t overlay, write_only image2d_t dst, float xScale, float yScale)
{
	int2 srcPos={get_global_id(0), get_global_id(1)};
//	int2 overlayPos=(int2){ srcPos.x*xScale, srcPos.y*yScale };
    float2 scale={xScale, yScale};
    float2 overlayPos=convert_float2(srcPos)*scale;

	uint4 srcValue=read_imageui(src, nearestSampler, srcPos);
	uint4 overlayValue=read_imageui(overlay, linearSampler, overlayPos);
	uint4 dstValue;
	
//	dstValue.xyz=(overlayValue.xyz*overlayValue.w)+(srcValue.xyz*(1.0-overlayValue.w));
    dstValue.xyz=((overlayValue.xyz*overlayValue.w)+(srcValue.xyz*(255-overlayValue.w)))/255;
    dstValue.w=255;

	write_imageui(dst, srcPos, dstValue);
}