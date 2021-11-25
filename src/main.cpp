#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "util.hpp"
#include "config.hpp"

int main(int argc, char* argv[])
{
	spdlog::stopwatch sw;
	const auto config = init(argc,argv);

	srcFiles srcs = getSrcFiles(config);
	
	for(auto& target : config.targets)
	{

		spdlog::info("creating target: {}",target.name);
		srcFiles targerSrcs = filterSrcs(srcs,target.excludes);

		spdlog::trace("target srcs\n{}",targerSrcs);

		const auto name = target.name;
		if(target.makeFolder)
			createTarget(target,targerSrcs,config.outDir/name);
		if(target.makeArchive)
		{
			auto out = config.outDir/(name+"_V"+config.version+".zip");
			createArchive(target,targerSrcs,out);
		}
	}

	spdlog::info("build finished in {:.3f} seconds",sw.elapsed().count());
	return EXIT_SUCCESS;
}