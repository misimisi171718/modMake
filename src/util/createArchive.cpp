#include <miniz.h>
#include <spdlog/spdlog.h>

#include "../util.hpp"

void createArchive(const Target& target, const srcFiles& files, const fs::path& outName)
{

	spdlog::info("	- archive");

	fs::remove(outName);

	mz_zip_archive arc;
	memset(&arc,0,sizeof(mz_zip_archive));
	mz_zip_writer_init_file(&arc,outName.c_str(),1024*1024);

	for (auto &file : files.modFiles)
		mz_zip_writer_add_file(&arc,("mods"/file.filename()).c_str(),file.c_str(),"",1,0);
	
	for (auto &file : files.configFiles)
		mz_zip_writer_add_file(&arc,("config"/file.filename()).c_str(),file.c_str(),"",1,0);

	for (auto &folder : files.configFolders)
		for (auto &file : fs::directory_iterator(folder))
		{
			if(!file.is_regular_file())
				continue;
			const auto f = file.path();
			mz_zip_writer_add_file(&arc,("config"/f).c_str(),f.c_str(),"",1,0);
		
		}
		

	mz_zip_writer_finalize_archive(&arc);
	mz_zip_writer_end(&arc);
}