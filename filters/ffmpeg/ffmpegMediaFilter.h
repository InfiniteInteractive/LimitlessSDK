#ifndef _FfmpegMediaFilter_h_
#define _FfmpegMediaFilter_h_

#include "Media/IMediaFilter.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
}

class FfmpegMediaFilter:public Limitless::IMediaFilter
{
public:
	FfmpegMediaFilter(std::string name, Limitless::SharedMediaFilter parent):Limitless::IMediaFilter(name, parent){};
	~FfmpegMediaFilter(){};

protected:
	void buildAttribute(AVCodecContext *object, const AVOption *option, std::string prefix)
	{
		void *dst=((uint8_t*)object)+option->offset;
		std::string attributeName=prefix+option->name;

		switch(option->type)
		{
		case AV_OPT_TYPE_FLAGS:
		case AV_OPT_TYPE_INT:
			{
				int value=*(int *)dst;

				if(value != (int)option->default_val.i64)
					value=(int)option->default_val.i64;

				addAttribute(attributeName, value);
			}
			break;
			//	case AV_OPT_TYPE_PIXEL_FMT:
			//	case AV_OPT_TYPE_SAMPLE_FMT:
		case AV_OPT_TYPE_INT64:
			{
				__int64 value=*(__int64 *)dst;

				if(value != option->default_val.i64)
					value=option->default_val.i64;
				
				addAttribute(attributeName, value);
				
			}
			break;
		case AV_OPT_TYPE_FLOAT:
			{
				float value=*(float *)dst;

				if(value != (float)option->default_val.dbl)
					value=(float)option->default_val.dbl;

				addAttribute(attributeName, value);

			}
			break;
		case AV_OPT_TYPE_DOUBLE:
			{
				double value=*(double *)dst;

				if(value != option->default_val.dbl)
					value=option->default_val.dbl;

				addAttribute(attributeName, value);

			}
			break;
		case AV_OPT_TYPE_STRING:
			{
				char *value=*(char **)dst;

				if(option->default_val.str != NULL)
				{
					if(strcmp(value, option->default_val.str) != 0)
						strcpy(value, option->default_val.str);
				}
				if(value != NULL)
					addAttribute(attributeName, value);
				else
					addAttribute(attributeName, "");
			}
			//	case AV_OPT_TYPE_RATIONAL:  *intnum = ((AVRational*)dst)->num;
			//	                            *den    = ((AVRational*)dst)->den;
			//	                                                    return 0;
			//	case AV_OPT_TYPE_CONST:     *num    = o->default_val.dbl; return 0;
		}
	}
};



#endif //_FfmpegResources_h_