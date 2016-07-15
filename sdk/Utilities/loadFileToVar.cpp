#include "Utilities/loadFileToVar.h"
#include <boost/regex.hpp>
#include <stdio.h>

#ifndef WIN32
#pragma warning(push)
#pragma warning(disable:4512)
#endif
#include <boost/filesystem.hpp>
#ifndef WIN32
#pragma warning(pop)
#endif

std::string Limitless::loadFileToString(std::string path, std::string fileName)
{
	std::string buffer;
	std::string filePath=path+'/'+fileName;
	FILE *file=fopen(filePath.c_str(), "r");

	if(file != NULL)
	{
		int capacity=1024;
		char *localBuffer=(char *)malloc(capacity);
		boost::regex expression("^\\s*#include\\s*[\"<]\\s*([\\w.]+)\\s*[\">]\\s*?");

		while(fgets(localBuffer, capacity, file) != NULL)
		{
			while(strlen(localBuffer) == capacity-1)
			{
				if(localBuffer[capacity-2] == '\n')
					break;

				localBuffer=(char *)realloc(localBuffer, capacity+1024);

				if(localBuffer == NULL)
					return buffer;

				if(fgets(&localBuffer[capacity-1], 1025, file) == NULL)
					break;
				capacity+=1024;
			}

			boost::cmatch match;

			if(regex_match(localBuffer, match, expression))
				buffer+=loadFileToString(path, match[1]);
			else
				buffer+=localBuffer;
		}
		fclose(file);
		free(localBuffer);
	}
	return buffer;
}

std::string Limitless::loadResource(std::string externVariable, std::string fileName)
{
        std::string fileContent;
		boost::filesystem::path path(fileName);

		fileContent=loadFileToString(path.parent_path().string(), path.filename().string());

		if(fileContent.empty())
			return externVariable;
		return fileContent;
}