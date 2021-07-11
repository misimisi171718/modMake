#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "util.hpp"

int main(int argc, char* argv[])
{
	spdlog::stopwatch sw;
	auto config = init(argc,argv);

	auto srcDirs = config["srcDirs"];
	srcFiles srcFiles;
	for(auto i : srcDirs)
		srcFiles += getSrcFiles(i.as<std::string>());

	for(auto target : config["targets"])
	{
		auto name = target.first.as<std::string>();
		createTarget(name,srcFiles,config);
	}

	spdlog::info("build finished in {:.3f} seconds",sw.elapsed().count());
	return EXIT_SUCCESS;
}