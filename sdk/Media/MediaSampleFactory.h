#ifndef _MediaSampleFactory_h_
#define _MediaSampleFactory_h_

#include "Media/media_define.h"
#include "Base/PluginFactory.h"
#include "Media/SampleType.h"
//#include "Media/MediaSample.h"

#include <vector>

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{
class MediaSample;
typedef boost::shared_ptr<MediaSample> SharedMediaSample;

//work around for VS2013, works for Clang as well
template <typename T>
struct has_s_type
{
	struct Fallback { SampleType s_type; };
	struct Derived: T, Fallback {};

	template<typename C, C> struct ChT;
	template<typename C> static char(&f(ChT<SampleType Fallback::*, &C::s_type>*))[1];
	template<typename C> static char(&f(...))[2];

	static bool const value=sizeof(f<Derived>(0)) == 2;
};
//end work around

class MediaSampleFactory;

template<typename CLASS, typename INTERFACE, bool _InterfaceClass=false>
class AutoRegisterMediaSample:public INTERFACE
{
public:
	AutoRegisterMediaSample():INTERFACE() { &s_type; }

//	static MediaSample *create(unsigned int type) { CLASS *newClass=new CLASS(); newClass->setType(s_type); return newClass; }

//	template<typename T, typename=void>
//	struct BaseId
//	{
//		static std::vector<unsigned int> get() { return std::vector<unsigned int>(); }
//	};
//
//	template<typename T>
//	struct BaseId<T, decltype((void)T::s_type, void())>
//	{
//		static std::vector<unsigned int> get()
//		{
//			std::vector<unsigned int> ids;
//			ids.push_back(T::s_type.id); ids.insert(ids.end(), T::s_type.idInheritance.begin(), T::s_type.idInheritance.end());
//			return ids;
//		}
//	};
//
//	static std::vector<unsigned int> getBaseIds() { return BaseId<INTERFACE>::get(); }

//work around for VS2013, works for Clang as well. Above code likely better when bugs fixed in VS2013
	template<typename T, bool=false> //typename T here as Clang does not like explicit instantation in a class context
	struct BaseInterface
	{
		static std::vector<unsigned int> getIds() { return std::vector<unsigned int>(); }
	};

	template<typename T>
	struct BaseInterface<T, true>
	{
		static std::vector<unsigned int> getIds()
		{
			std::vector<unsigned int> ids;
//			ids.push_back(INTERFACE::s_type.id); ids.insert(ids.end(), INTERFACE::s_type.idInheritance.begin(), INTERFACE::s_type.idInheritance.end());
			ids.push_back(INTERFACE::getId()); std::vector<unsigned int> baseIds=INTERFACE::getIds(); ids.insert(ids.end(), baseIds.begin(), baseIds.end());
			return ids;
		}
	};
//end work around
	template<typename T>
	static typename std::enable_if<std::is_abstract<T>::value, MediaSample *>::type createClass(unsigned int type) { static_assert(_InterfaceClass, "Auto registered type that is abstract (has pure virtual functions) without declaring it as an interface"); return nullptr; }
	
	template<typename T>
	static typename std::enable_if<!std::is_abstract<T>::value, MediaSample *>::type createClass(unsigned int type) { CLASS *newClass=new CLASS(); newClass->setType(s_type); return newClass; }
	
	static MediaSample *create(unsigned int type) { return createClass<CLASS>(type); }

	static unsigned int getId() { return s_type.id; }
	static std::vector<unsigned int> getIds() { return s_type.idInheritance; }

	static std::vector<unsigned int> getBaseIds() { return BaseInterface<INTERFACE, has_s_type<INTERFACE>::value>::getIds(); }

private:
	static const SampleType s_type;
};

class MEDIA_EXPORT MediaSampleFactory
{
	typedef MediaSample *(*FactoryFunc)(unsigned int type);
	class MediaSampleNode
	{
	public:
		MediaSampleNode(unsigned int type, std::string typeName):
			m_type(type), m_typeName(typeName), m_factoryFunction(nullptr){};
		MediaSampleNode(unsigned int type, std::string typeName, FactoryFunc factoryFunction):
			m_type(type), m_typeName(typeName), m_factoryFunction(factoryFunction){};

		size_t type(){return m_type;}
		std::string typeName(){return m_typeName;}
		MediaSample *factoryFunction(){return m_factoryFunction(m_type);}

	private:
		unsigned int m_type;
		std::string m_typeName;
		FactoryFunc m_factoryFunction;
	};
	typedef std::vector<MediaSampleNode> MediaSampleNodes;

public:
	MediaSampleFactory(){}
	~MediaSampleFactory(){}

public:
	static size_t getTypeId(std::string typeName);
//	static std::vector<std::string> getType(std::string type);
	static void deleteSample(MediaSample *sample);

	static SharedMediaSample createType(std::string typeName, std::function<void(MediaSample *)> deleteFunction=deleteSample);
	static SharedMediaSample createType(unsigned int type, std::function<void(MediaSample *)> deleteFunction=deleteSample);

	template<typename Type>	
	static boost::shared_ptr<Type> create(std::function<void(MediaSample *)> deleteFunction=deleteSample)
	{
		unsigned int type=Type::getId();

		SharedMediaSample sample=createType(type, deleteFunction);

		return boost::dynamic_pointer_cast<Type>(sample);
	}

//	static SharedMediaSample createType(std::string typeName, std::function<void(MediaSample *)> deleteFunction=[](MediaSample *mediaSample) { delete mediaSample; });
//	static SharedMediaSample createType(unsigned int type, std::function<void(MediaSample *)> deleteFunction=[](MediaSample *mediaSample) { delete mediaSample; });

	//Setup devices
//	static std::string registerType(std::string typeName, FactoryFunc factoryFunc);
	static SampleType registerType(std::string typeName, std::vector<unsigned int> ids, FactoryFunc factoryFunc);
//	{
//		s_nodes.s_mediaSampleNodes.push_back(MediaSampleNode(s_nodes.s_sampleTypeIndex, typeName, factoryFunc));
//		s_nodes.s_sampleTypeIndex++;
//		return typeName;
//	}

	struct MediaSampleNodeHolder //keeps vector from be reset by Dll inits
	{
		MediaSampleNodeHolder():s_sampleTypeIndex(0) {}

		MediaSampleNodes s_mediaSampleNodes;
		unsigned int s_sampleTypeIndex;
	};

private:
//	static MediaSampleNodeHolder s_nodes;
//	static MediaSampleNodes s_mediaSampleNodes;
//	static unsigned int s_sampleTypeIndex;
};

//template<typename CLASS, typename INTERFACE> std::string AutoRegisterMediaSample<CLASS, INTERFACE>::s_typeName=\
//MediaSampleFactory::registerType(TypeName<CLASS>::get(), &AutoRegisterMediaSample<CLASS, INTERFACE>::create);

//template<typename CLASS, typename INTERFACE, typename DEFAULT> SampleType AutoRegisterMediaSample<CLASS, INTERFACE, DEFAULT>::s_type=\
//MediaSampleFactory::registerType(TypeName<CLASS>::get(), AutoRegisterMediaSample<CLASS, INTERFACE, DEFAULT>::getBaseIds(), &AutoRegisterMediaSample<CLASS, INTERFACE, DEFAULT>::create);

//template<typename CLASS, typename INTERFACE, typename DEFAULT> SampleType AutoRegisterMediaSample<CLASS, INTERFACE, decltype((void)INTERFACE::s_type, 0)>::s_type=\
//MediaSampleFactory::registerType(TypeName<CLASS>::get(), AutoRegisterMediaSample<CLASS, INTERFACE, decltype((void)INTERFACE::s_type, 0)>::getBaseIds(), &AutoRegisterMediaSample<CLASS, INTERFACE, decltype((void)INTERFACE::s_type, 0)>::create);
//template<typename CLASS, typename INTERFACE> SampleType AutoRegisterMediaSample<CLASS, INTERFACE, int>::s_type=\
//MediaSampleFactory::registerType(TypeName<CLASS>::get(), AutoRegisterMediaSample<CLASS, INTERFACE>::getBaseIds(), &AutoRegisterMediaSample<CLASS, INTERFACE>::create);

template<typename CLASS, typename INTERFACE, bool _InterfaceClass> const SampleType AutoRegisterMediaSample<CLASS, INTERFACE, _InterfaceClass>::s_type=\
MediaSampleFactory::registerType(TypeName<CLASS>::get(), AutoRegisterMediaSample<CLASS, INTERFACE, _InterfaceClass>::getBaseIds(), &AutoRegisterMediaSample<CLASS, INTERFACE, _InterfaceClass>::create);

//template<typename CLASS, typename INTERFACE> SampleType AutoRegisterMediaSample<CLASS, INTERFACE, int>::s_type=SampleType();

}//namespace Limitless

#pragma warning(pop)

#endif //_MediaSampleFactory_h_