#include <memory>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <tclap/CmdLine.h>

#include "../util.hpp"

static void checkConfig(const YAML::Node& config)
{
	if(! config.IsMap())
	{
		spdlog::critical("invalid config file worng base data type");
		std::exit(EXIT_FAILURE);
	}

	const std::unordered_set<std::string> configNames = {
		"name",
		"version",
		"srcDirs",
		"outDir",
		"mixedModsAndConfig",
		"targets"
	};
	for(auto i : config)
	{
		const auto item = i.first.as<std::string>();
		if(configNames.find(item) == configNames.end());
			spdlog::warn("unkown config item \"{}\" it will be ignored", item);
	}
}

static YAML::Node loadConfig(const fs::path& confFile)
{
	if(!fs::exists(confFile))
	{
		spdlog::critical("no {} found please chek if you are in the right directory",confFile.c_str());
		std::exit(EXIT_FAILURE);
	}
	return YAML::LoadFile(confFile);
}

YAML::Node init(int argc,char* argv[])
{
	try {  
		TCLAP::CmdLine cmd("", ' ', "0.1");
		TCLAP::ValueArg<std::string> rootLocation("r" ,"root", "root folder for project", false, ".", "rel/abs path",cmd);
		TCLAP::ValueArg<std::string> configLocation("c" ,"config", "path to the config file", false, "modMake.yaml", "rel/abs path",cmd);
		TCLAP::SwitchArg verbose("v", "verbose", "print more build information", cmd, false);
		cmd.parse( argc, argv );


		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
		consoleSink->set_level(verbose.getValue() ? spdlog::level::trace : spdlog::level::debug);

		auto main = std::make_shared<spdlog::logger>("Main",consoleSink);
		main->set_level(spdlog::level::trace);
		spdlog::set_default_logger(main);

		const fs::path confFile = configLocation.getValue();
		YAML::Node config;
		if(configLocation.isSet())
		{
			config = loadConfig(confFile);
			fs::current_path(rootLocation.getValue());
		}
		else
		{
			fs::current_path(rootLocation.getValue());
			config = loadConfig(confFile);
		}

		checkConfig(config);

		fs::path outDir = config["outDir"].as<std::string>();
		fs::create_directories(outDir);

		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(outDir/"modMake.log",true);
		fileSink->set_level(spdlog::level::trace);
		main->sinks().push_back(fileSink);


		return config;
	} catch (TCLAP::ArgException &e)  // catch exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	std::exit(EXIT_FAILURE);
	return {};
}