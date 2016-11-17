hunter_config(Eigen VERSION 3.2.4-p0)
hunter_config(Jpeg VERSION 9b-p1)
hunter_config(PNG VERSION 1.6.16-p4)
hunter_config(glm VERSION 0.9.7.6)
hunter_config(TIFF VERSION 4.0.2-p3)
hunter_config(ZLIB VERSION 1.2.8-p3)
hunter_config(RapidJSON VERSION 1.0.2-p2)

hunter_config(dlib VERSION 19.0-p2
CMAKE_ARGS
#    BUILD_SHARED_LIBS=ON
    DLIB_HEADER_ONLY=OFF
    DLIB_ENABLE_ASSERTS=ON
)

hunter_config(Boost VERSION 1.61.0
CMAKE_ARGS 
    BUILD_SHARED_LIBS=ON 
)

hunter_config(OpenCV VERSION 3.1.0-p2 
CMAKE_ARGS 
    BUILD_SHARED_LIBS=ON 
    WITH_OPENCL=ON
    WITH_CUDA=OFF
)

hunter_config(OpenCV-Extra VERSION 3.0.0)