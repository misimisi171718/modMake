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

void createTarget(const std::string& name, const srcFiles& files, const YAML::Node& config) 
{
	spdlog::info("creating target: {}",name);
	auto settings = config["targets"][name];
	auto base = fs::path( config["outDir"].as<std::string>() )/name;

	std::vector<std::regex> excludes;
	if(settings["exclude"])
		for(auto i:settings["exclude"])
		{
			std::regex excludeRE(i.as<std::string>(),std::regex::optimize);
			excludes.push_back(excludeRE);
		}

	fs::remove_all(base);

	auto modsDir = base/"mods";
	fs::create_directories(modsDir);
	copyFiles(files.modFiles,modsDir);

	auto configDir = base/"config";
	fs::create_directories(configDir);
	copyFiles(files.configFiles,configDir);
	copyFiles(files.configFolders,configDir);
}
