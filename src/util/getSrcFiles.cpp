#include <tuple>

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

static void removeConfigPatternFolders(std::vector<fs::path>& folders,const std::vector<std::regex>& configPat)
{
	for (auto &re : configPat)
		for (int i = folders.size() - 1; i >= 0; i--)
		{
			auto s = folders[i].string();
			if(std::regex_search(s,re))
			{
				folders.erase(folders.begin() + i);
			}
		}
}

static void filterConfigs(srcFiles& files)
{
	auto& folders = files.configFolders;

	std::vector<int> toRemove;
	toRemove.reserve(folders.size());
	for (int rloc = folders.size() - 1; rloc >= 0; rloc--)
	{
		for (int i = rloc - 1; i >= 0 ; i--)
		{
			if(std::find(toRemove.begin(),toRemove.end(),i) != toRemove.end())
				break;

			if(folders[i].string().find(folders[rloc]) == 0)
				toRemove.push_back(i);
		}
	}
	for (auto &i : toRemove)
	{
		auto temp = folders.begin() + i;
		spdlog::trace("deleted extra folder: {}",temp->c_str());
		folders.erase(temp);
	}
	


	auto& confFiles = files.configFiles;
	for(auto& folder:folders)
		for(int i = confFiles.size()-1; i>=0; i--)
			if(confFiles[i].string().find(folder/"") == 0)
			{
				auto temp = confFiles.begin() + i;
				spdlog::trace("deleted extra config file: {}",temp->c_str());
				confFiles.erase(temp);
			}

}

static void removeDuplicates(std::vector<fs::path>& files)
{
	std::vector<std::pair<int,int>> toRemove;
	toRemove.reserve(files.size());
	
	for (size_t i = 0; i < files.size(); i++)
		for (size_t j = i+1; j < files.size(); j++)
			if(files[i].filename() == files[j].filename())
				toRemove.push_back({i,j});
	
	std::sort(toRemove.begin(),toRemove.end(),[](auto x, auto y){return x.second > y.second;});



	for (auto &i : toRemove)
		spdlog::warn("duplicate files removing the second one\n\t{}\n\t{}",
			files[i.first].c_str(),
			files[i.second].c_str());

	for (auto &i : toRemove)
		files.erase(files.begin() + i.second);
	

}

srcFiles getSrcFiles(const Configuration& config) 
{
	auto& location = config.srcDir;
	if(not fs::exists(location))
		spdlog::error("source {} location dosent exist",location.c_str());
	
	srcFiles ret;

	exploreDir(location,ret);

	removeConfigPatternFolders(ret.configFolders,config.configPatterns);
	filterConfigs(ret);

	removeDuplicates(ret.modFiles);
	removeDuplicates(ret.configFiles);
	removeDuplicates(ret.configFolders);

	return ret;
}
