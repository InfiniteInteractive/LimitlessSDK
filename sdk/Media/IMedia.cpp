#include "IMedia.h"

#include <boost/filesystem.hpp>
#include "Media/mediaFactory.h"

namespace Limitless
{

std::shared_ptr<IMedia> requestLoad(std::string name, std::string location)
{
	SharedMedia media;
	boost::filesystem::path filePath(location);

	if(!boost::filesystem::exists(filePath))
		return media;

	std::string extension=filePath.extension().string().substr(1);
	std::vector<std::string> extHandlers=MediaFactory::instance().handlesExtension(extension);

	if(extHandlers.empty())
		return media;

	media=MediaFactory::instance().create(extHandlers[0]);

	media->load(filePath.string());
	return media;
}

}