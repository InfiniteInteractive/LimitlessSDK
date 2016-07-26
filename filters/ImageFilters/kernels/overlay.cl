__constant sampler_t nearestSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

kernel void overlay(read_only image2d_t src, read_only image2d_t overlay, write_only image2d_t dst, float xScale, float yScale)
{
	int2 srcPos={get_global_id(0), get_global_id(1)};
	int2 overlayPos=srcPos*(int2) { xScale, yScale };
	uint4 srcValue=read_imageui(src, nearestSampler, srcPos);
	uint4 overlayValue=read_imageui(overlay, nearestSampler, overlayPos);
	uint4 dstValue;
	
//	dstValue.xyz=(overlayValue.xyz*overlayValue.w)+(srcValue.xwz*(1.0-overlayValue.w));
    dstValue.xyz=((overlayValue.xyz*overlayValue.w)+(srcValue.xwz*(255-overlayValue.w)))/255;
    dstValue.w=1.0;
	write_imageui(dst, srcPos, dstValue);
}