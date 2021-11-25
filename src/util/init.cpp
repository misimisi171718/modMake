#include <memory>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <tclap/CmdLine.h>

#include "../util.hpp"

template<typename T>
static T getOpt(const YAML::Node& conf,const std::string& name,T def,bool warn)
{
	if(!conf[name])
	{
		if(warn)
			spdlog::warn("failed to get options \"{}\" using default \"{}\"",name,std::string{def});
		return def;
	}
	return conf[name].as<T>();
}

template<typename T>
static T requireOpt(const YAML::Node& conf,const std::string& name)
{
	if(!conf[name])
	{
		spdlog::critical("failed to get required option named \"{}\"",name);
		std::exit(EXIT_FAILURE);
	}
	return conf[name].as<T>();
}

template<typename T>
static std::vector<T> requireOptAsVec(const YAML::Node& conf, const std::string& name, const std::function<T(const YAML::const_iterator)>& process)
{
	if(!conf[name])
	{
		spdlog::critical("no \"{}\" are defined", name);
		std::exit(EXIT_FAILURE);
	}

	if(!conf[name].IsMap() && !conf[name].IsSequence())
	{
		spdlog::critical("paramter named \"{}\" is not type map or sequence",name);
		std::exit(EXIT_FAILURE);
	}

	std::vector<T> ret;
	ret.reserve(conf[name].size());

	for (auto i = std::begin(conf[name]); i != std::end(conf[name]); i++)
	{
		ret.push_back(process(i));
	}
	
	return ret;
}
template<typename T>
static std::vector<T> getOptAsVec(const YAML::Node& conf, const std::string& name, const bool warn, const std::function<T(const YAML::const_iterator)>& process)
{
	if(!conf[name])
	{
		if(warn)
			spdlog::warn("failed to get option \"{}\" using default",name);
		return {};
	}

	if(!conf[name].IsMap() && !conf[name].IsSequence())
	{
		spdlog::critical("paramter named \"{}\" is not type map or sequence",name);
		std::exit(EXIT_FAILURE);
	}

	std::vector<T> ret;
	ret.reserve(conf[name].size());

	for (auto i = std::begin(conf[name]); i != std::end(conf[name]); i++)
	{
		ret.push_back(process(i));
	}
	
	return ret;
}

static Configuration loadConfig(const fs::path& confFile)
{

	if(!fs::exists(confFile))
	{
		spdlog::critical("no {} found please chek if you are in the right directory",confFile.c_str());
		std::exit(EXIT_FAILURE);
	}
	Configuration ret;
	auto file = YAML::LoadFile(confFile);

	ret.outDir  = getOpt<std::string>(file,"outDir","build",false);
	ret.srcDir  = requireOpt<std::string>(file,"srcDir");
	ret.version = getOpt<std::string>(file,"version","",true);
	
	ret.targets = requireOptAsVec<Target>(file,"targets",[](const YAML::const_iterator conf){
		Target ret;
		ret.name = conf->first.as<std::string>();
		ret.makeFolder = getOpt(conf->second,"makeFolder",true,true);
		ret.makeArchive = getOpt(conf->second,"makeArchive",false,true);
		ret.excludes = requireOptAsVec<std::regex>(conf->second,"exclude",[](const YAML::const_iterator ex){
			std::regex ret{ex->as<std::string>(),std::regex::optimize};
			return ret;
		});
		return ret;
	});

	ret.configPatterns = getOptAsVec<std::regex>(file,"configPatterns",false,[](const YAML::const_iterator ex){
		auto s = ex->as<std::string>();
		std::regex ret{s,std::regex::optimize};
		return ret;
	});

	return ret;
}

Configuration init(int argc,char* argv[])
{
	try {  
		TCLAP::CmdLine cmd("", ' ', "0.1");
		TCLAP::ValueArg<std::string> rootLocation("r" ,"root", "root folder for project", false, ".", "rel/abs path",cmd);
		TCLAP::ValueArg<std::string> configLocation("c" ,"config", "path to the config file", false, "./modMake.yaml", "rel/abs path",cmd);
		TCLAP::SwitchArg verbose("v", "verbose", "print more build information", cmd, false);
		cmd.parse( argc, argv );


		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
		consoleSink->set_level(verbose.getValue() ? spdlog::level::trace : spdlog::level::debug);

		auto main = std::make_shared<spdlog::logger>("Main",consoleSink);
		main->set_level(spdlog::level::trace);
		spdlog::set_default_logger(main);

		const fs::path confFile = configLocation.getValue();
		Configuration config;

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


		fs::create_directories(config.outDir);

		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(config.outDir/"modMake.log",true);
		fileSink->set_level(spdlog::level::trace);
		main->sinks().push_back(fileSink);

		return config;
	} catch (TCLAP::ArgException &e)  // catch exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	std::exit(EXIT_FAILURE);
	return {};
}