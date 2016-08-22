#ifndef _AudioControls_medialibHelpers_h_
#define _AudioControls_medialibHelpers_h_

#include "medialib/audioFormat.h"
#include "Media/IAudioSample.h"

medialib::AudioFormat convertFormat(Limitless::AudioSampleFormat format);
Limitless::AudioSampleFormat convertFormat(medialib::AudioFormat format);

#endif // _AudioControls_medialibHelpers_h_
