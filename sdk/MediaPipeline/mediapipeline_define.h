#ifndef MEDIAPIPELINE_DEFINE_H
#define MEDIAPIPELINE_DEFINE_H



#ifdef MediaPipeline_EXPORTS
# define MEDIAPIPELINE_EXPORT __declspec(dllexport)
#else
# define MEDIAPIPELINE_EXPORT __declspec(dllimport)
#endif

#endif // MEDIAPIPELINE_DEFINE_H
