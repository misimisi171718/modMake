#include <regex>

#include <spdlog/spdlog.h>

#include "../util.hpp"

static void copyFiles(const std::vector<fs::path>& src,const fs::path& dest)
{
	for(auto& i: src)
	{
		try{
			fs::copy(i,dest,fs::copy_options::recursive);
		}catch(const std::exception& e){
			spdlog::warn("duplicate file at \"{}\"",i.c_str());
		}
	}
}

void createTarget(const Target& target, const srcFiles& files, const fs::path& outDir )
{
	spdlog::info("	- folder");

	fs::remove_all(outDir);

	auto modsDir = outDir/"mods";
	fs::create_directories(modsDir);
	copyFiles(files.modFiles,modsDir);

	auto configDir = outDir/"config";
	fs::create_directories(configDir);
	copyFiles(files.configFiles,configDir);
	copyFiles(files.configFolders,configDir);
}
