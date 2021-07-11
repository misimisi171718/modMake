#include <spdlog/spdlog.h>

#include "../util.hpp"

static bool exploreDir(const fs::path& path, srcFiles& files)
{
	bool hasJarFiles = false;
	for(auto& i: fs::directory_iterator(path))
	{
		if(i.is_directory())
		{
			const bool isConfigDir = exploreDir(i,files);
			if(!isConfigDir)
				hasJarFiles = true;
			else
			{
				const fs::path filePath = i.path();
				files.configFolders.push_back(filePath);
				spdlog::trace("added config folder: {} ",filePath.c_str());
			}
		}
		else
		{
			const fs::path filePath = i.path();
			if(filePath.extension() == ".jar")
			{
				hasJarFiles = true;
				files.modFiles.push_back(filePath);
				spdlog::trace("added mod: {} ",filePath.c_str());
			}
			else
			{
				files.configFiles.push_back(filePath);
				spdlog::trace("added config file: {} ",filePath.c_str());
			}

		}
	}
	return !hasJarFiles;
}

static void filterConfigs(srcFiles& files)
{
	auto& folders = files.configFolders;
	bool removedFolder = true;
	size_t reversLocation = 0;
	while(removedFolder)
	{
		if(reversLocation >= folders.size())
			break;

		std::vector<int> removeLocations;
		auto toRemove = *(folders.rbegin()-reversLocation);
		for(int i = folders.size()-1; i >= 0; i--)
			if(folders[i].string().find(toRemove) == 0 && folders[i] != toRemove)
				removeLocations.push_back(i);
		
		for(auto i:removeLocations)
		{
			auto temp = folders.begin() + i;
			spdlog::trace("deleted extra folder: {}",temp->c_str());
			folders.erase(temp);
		}
		
		if(removeLocations.empty())
			removedFolder = false;
		reversLocation++;
	}


	auto& confFiles = files.configFiles;
	for(auto& folder:folders)
		for(int i = confFiles.size()-1; i>=0; i--)
			if(confFiles[i].string().find(folder) == 0)
			{
				auto temp = confFiles.begin() + i;
				spdlog::trace("deleted extra config file: {}",temp->c_str());
				confFiles.erase(temp);
			}

}

srcFiles getSrcFiles(const fs::path& location) 
{
	if(not fs::exists(location))
		spdlog::error("source {} location dosent exist",location.c_str());
	
	srcFiles ret;

	exploreDir(location,ret);
	filterConfigs(ret);

	return ret;
}
