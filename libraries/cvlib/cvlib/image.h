#ifndef _cvlib_image_h_
#define _cvlib_image_h_

#include <stdint.h>
#include <memory>

namespace cvlib
{

enum class ImageFormat
{
    Binary,
    GreyScale, //grey scale 8 bit
	GreyScale32f,//grey scale 32 bit float
    Ra, //Ra 8bit per channel, 16bit
    Rgb, //Rgb 8bit per channel, 24bit
    Rgba //Rgba 8bit per channel, 32bit
};

#define CVLIB_CONCEPT_POLYMORPHISM

#ifdef CVLIB_CONCEPT_POLYMORPHISM
class Image
{
public:
    template<typename _Type>
    Image(_Type &value):m_self(new ImageModel<_Type>(value)) {}

    bool resize(ImageFormat format, size_t width, size_t height) const{ return m_self->resize(format, width, height); }

    size_t width() const{ return m_self->width(); }
    size_t height() const { return m_self->height(); }
    size_t stride() const { return m_self->stride(); }

    ImageFormat format() const { return m_self->format(); }

    uint8_t *data() const { return m_self->data(); }
    size_t dataSize() const { return m_self->dataSize(); }

private:
    struct ImageConcept
    {
        virtual ~ImageConcept()=default;

        virtual bool resize(ImageFormat format, size_t width, size_t height) const=0;
        virtual size_t width() const=0;
        virtual size_t height() const=0;
        virtual size_t stride() const=0;

        virtual ImageFormat format() const=0;

        virtual uint8_t *data() const=0;
        virtual size_t dataSize() const=0;
    };

    template<typename _Type>
    struct ImageModel:ImageConcept
    {
        ImageModel(_Type &value):m_data(value) {}

        virtual bool resize(ImageFormat format, size_t width, size_t height) const { return cvlib::resize(m_data, format, width, height); }

        virtual size_t width() const { return cvlib::width(m_data); }
        virtual size_t height() const { return cvlib::height(m_data); }
        virtual size_t stride() const { return cvlib::stride(m_data); }
        
        virtual ImageFormat format() const{ return cvlib::format(m_data); }
        
        virtual uint8_t *data() const { return cvlib::data(m_data); }
        virtual size_t dataSize() const { return cvlib::dataSize(m_data); }

        _Type &m_data;
    };

    std::shared_ptr<const ImageConcept> m_self;
};

namespace traits
{

template<ImageFormat _PixelType>
size_t sizeOf();

template<>
inline size_t sizeOf<ImageFormat::Binary>() { return sizeof(uint8_t); }

template<>
inline size_t sizeOf<ImageFormat::GreyScale>() { return sizeof(uint8_t); }

template<>
inline size_t sizeOf<ImageFormat::GreyScale32f>() { return sizeof(float); }

template<>
inline size_t sizeOf<ImageFormat::Ra>() { return sizeof(uint8_t)*2; }

template<>
inline size_t sizeOf<ImageFormat::Rgb>() { return sizeof(uint8_t)*3; }

template<>
inline size_t sizeOf<ImageFormat::Rgba>() { return sizeof(uint8_t)*4; }

} //namespace traits

size_t sizeOfImageFormat(ImageFormat format);

namespace utils
{

template<ImageFormat _PixelType>
bool comparePixel(uint8_t *src1Data, uint8_t *src2Data);

template<>
inline bool comparePixel<ImageFormat::Binary>(uint8_t *src1Data, uint8_t *src2Data) { return (*src1Data)==(*src2Data); }

template<>
inline bool comparePixel<ImageFormat::GreyScale>(uint8_t *src1Data, uint8_t *src2Data) { return (*src1Data)==(*src2Data); }

template<>
inline bool comparePixel<ImageFormat::GreyScale32f>(uint8_t *src1Data, uint8_t *src2Data) { return (*(float *)(src1Data))==(*(float *)(src2Data)); }

template<>
inline bool comparePixel<ImageFormat::Ra>(uint8_t *src1Data, uint8_t *src2Data) { return (src1Data[0]==src2Data[0])&&(src1Data[1]==src2Data[1]); }

template<>
inline bool comparePixel<ImageFormat::Rgb>(uint8_t *src1Data, uint8_t *src2Data) { return (src1Data[0]==src2Data[0])&&(src1Data[1]==src2Data[1])&&(src1Data[2]==src2Data[2]); }

template<>
inline bool comparePixel<ImageFormat::Rgba>(uint8_t *src1Data, uint8_t *src2Data) { return (src1Data[0]==src2Data[0])&&(src1Data[1]==src2Data[1])&&(src1Data[2]==src2Data[2])&&(src1Data[2]==src2Data[2]); }

} //namespace support

#else //CVLIB_CONCEPT_POLYMORPHISM

//using image type
#endif //CVLIB_CONCEPT_POLYMORPHISM

}//namespace cvlib

#endif //_cvlib_image_h_