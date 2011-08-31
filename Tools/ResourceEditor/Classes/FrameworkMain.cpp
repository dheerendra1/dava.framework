//
//  main.m
//  TemplateProjectMacOS
//
//  Created by Vitaliy  Borodovsky on 3/16/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#include "DAVAEngine.h"
#include "GameCore.h"
#include "ResourcePackerScreen.h"
#include "TexturePacker/CommandLineParser.h"
using namespace DAVA;


void FrameworkDidLaunched()
{
	
	if (Core::Instance()->IsConsoleMode())
	{
		Vector<String> & commandLine = Core::Instance()->GetCommandLine();
		/*for (int k = 0; k < commandLine.size(); ++k)
		{
			printf("%s ", commandLine[k].c_str());
		}
		printf("\n");
		*/
		
		if (commandLine.size() < 2)
		{
			printf("Usage: resourcepacker [src_dir] [-v]\n");
			printf("\t-exo - extended output\n"); 
			printf("\t-v or --verbose - packer will give detailed output\n");
			return;
		}
		
		for (uint32 k = 0; k < commandLine.size(); ++k)
		{
			if ((commandLine[k] == "-v") || (commandLine[k] == "--verbose"))
				CommandLineParser::Instance()->SetVerbose(true);
			if (commandLine[k] == "-exo")
				CommandLineParser::Instance()->SetExtendedOutput(true);
		}
		//printf("\n"); 
		
		// CommandLineParser::Instance()->SetVerbose(true);
		// Logger::Instance()->SetLogLevel(Logger::LEVEL_DEBUG);
		
		ResourcePackerScreen * resourcePackerScreen = new ResourcePackerScreen();
		
		String path, lastDir;
		FileSystem::SplitPath(commandLine[1], path, lastDir);
		
		resourcePackerScreen->inputGfxDirectory = FileSystem::RealPath(commandLine[1]);// "/./../../ProjectDataSource/Gfx/");
		resourcePackerScreen->outputGfxDirectory = FileSystem::RealPath(resourcePackerScreen->inputGfxDirectory + "/../../Data/" + lastDir);
		resourcePackerScreen->excludeDirectory = FileSystem::RealPath(resourcePackerScreen->inputGfxDirectory + "/../");
		
		
		String excludeDirPath, excludeDirLastDir;
		FileSystem::SplitPath(resourcePackerScreen->excludeDirectory, excludeDirPath, excludeDirLastDir);
		if (excludeDirLastDir != "DataSource")
		{
			printf("[FATAL ERROR: Packer working only inside DataSource directory]");
			return;
		}


		uint64 elapsedTime = SystemTimer::Instance()->AbsoluteMS();
		printf("[Resource Packer Started]\n");
		printf("[INPUT DIR] - [%s]\n", resourcePackerScreen->inputGfxDirectory.c_str());
		printf("[OUTPUT DIR] - [%s]\n", resourcePackerScreen->outputGfxDirectory.c_str());
		printf("[EXCLUDE DIR] - [%s]\n", resourcePackerScreen->excludeDirectory.c_str());
		

		resourcePackerScreen->PackResources();
		elapsedTime = SystemTimer::Instance()->AbsoluteMS() - elapsedTime;
		printf("[Resource Packer Compile Time: %0.3lf seconds]\n", (float64)elapsedTime / 1000.0);

		SafeRelease(resourcePackerScreen);
		return;
	}	
	
#if defined(__DAVAENGINE_IPHONE__)
	KeyedArchive * appOptions = new KeyedArchive();
	appOptions->SetInt("orientation", Core::SCREEN_ORIENTATION_LANDSCAPE_LEFT);
	
	DAVA::Core::Instance()->SetVirtualScreenSize(480, 320);
	DAVA::Core::Instance()->RegisterAvailableResourceSize(480, 320, "XGfx");
#else
	KeyedArchive * appOptions = new KeyedArchive();
//	appOptions->SetInt("width",	920);
//	appOptions->SetInt("height", 690);

	appOptions->SetString("title", "DAVA SDK - Studio");
	appOptions->SetInt("width",	1024);
	appOptions->SetInt("height", 690);

	//appOptions->SetInt("fullscreen.width",	1280);
	//appOptions->SetInt("fullscreen.height", 800);
	
	appOptions->SetInt("fullscreen", 0);
	appOptions->SetInt("bpp", 32); 

	DAVA::Core::Instance()->SetVirtualScreenSize(1024, 690);
	DAVA::Core::Instance()->RegisterAvailableResourceSize(1024, 690, "XGfx");
#endif 
    
	GameCore * core = new GameCore();
	DAVA::Core::SetApplicationCore(core);
	DAVA::Core::SetOptions(appOptions);
}


void FrameworkWillTerminate()
{

}
