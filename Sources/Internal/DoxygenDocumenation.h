/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
/*
 This file is just for doxygen documentation.
 */


/**
	\mainpage DAVA SDK Documentation 
 
	This manual is divided in the following sections:
	- Introduction
		- \ref page_introduction
		- \ref page_getting_started
		- \ref page_framework_architecture
	- Reference
		- \ref core
		- \ref baseobjects
		- \ref math
	- Tutorials
		- \ref page_tutorial_packer
		- \ref page_tutorial_particles
	

	\page page_introduction Introduction

	Our SDK is one of the solutions that can help you to develop good games for big variety of platforms. 
	Main idea is easy cross-platform and cross-device game development.
	
	SDK is C/C++ based because of the following reasons: 
	<ul>
		<li>C/C++ available on almost every good platform</li>
		<li>Best performance for mobile platforms</li>
		<li>Tons of good existing libraries available for C/C++</li>
	</ul>
 
	\subsection sub_platforms Supported platforms / devices
	<ul>
		<li>Windows. Windows XP, Windows 7.</li>
		<li>Mac OS X 10.5+</li>
		<li>iOS 3.0+</li>
	</ul>
 
	\section target_audience Target audience
	DAVA SDK will be the best solution if your initial target platforms is iOS / Android / Mac OS X and PC. It will be good
	for casual game developers, indie game developers and all other developers who target their product to these platforms. 
	
	You can save your time on combining everything you need and just start work on the game logic immediatelly.	
		
	\section features Features
	<ul>
		<li><b>Base</b> - Base templates you normally need for development. Like Singleton, Reference counter, Message and EventDispatcher.</li>
		<li><b>Rendering abstraction</b> - It fast and efficient and support:
			 <ul>	
				<li>DirectX</li>
				<li>OpenGL and OpenGL ES</li>
			 </ul>
		</li>
		<li><b>2D Render objects</b>
			<ul>	
				<li><b>Sprites</b> - 2d objects with position, rotation, scale and pivot point on the screen</li>
				<li><b>TTF Fonts</b> - freetype integrated</li>
				<li><b>Graphics Fonts</b> - own tool to create graphics fonts</li>
				<li><b>Particle Systems</b> - own particle engine to create effects similar to Particle Illusion</li>
			</ul>
 		</li>
		<li><b>UI</b>
			<ul>	
				<li><b>Basic controls</b> - button / list / textfield / slider / static text</li>
				<li><b>Screen flow architecture</b> - architecture to simplify / diversify logic of your app flow</li>
				<li><b>Yaml</b> - loading of screen structure from yaml files</li>
			</ul>
		</li>
		<li><b>Math</b>
			 <ul>
				<li><b>Vector math</b> - everything you need in 2D & 3D</li>
				<li><b>Quaternions</b> - everything you can require for animations </li>
				<li><b>2D Primitives</b> - circle, polygon, rect, bounding box, and operations between them</li>
				<li><b>3D Primitives</b> - plane, frustum, bounding box and operations between them</li>
			 </ul>
		 </li>
		<li><b>Collision detection</b>
			 <ul>
				 <li><b>Basic 2D collisions</b> - SAT based collision detection system</li>
				 <li><b>Box2D integrated</b> - you can use Box2D if you more familiar with it</li>
			 </ul>
		</li>
		<li><b>Physics</b> - we've integrated Box2D for you to save your time.
		</li>
		<li><b>Filesystem utils</b>
			<ul>	
				<li><b>XML parser</b> - simple interface, so you do not need to learn libxml</li>
				<li><b>Yaml parser</b> - simple interface, and you do not need to learn libyaml</li>
				<li><b>Keyed Archives</b> - save / load files easily like in Cocoa</li>
			 </ul>
		</li>
		<li><b>Animation System</b> - cool animation system that allow to animate object properties easily. 
		</li>
		<li><b>Debug utils</b> can be used for rapid prototyping / error lookup
			<ul>
				<li>Ability to download configs from internet</li>
				<li>Memory manager to find memory leaks & overwrites</li>
			</ul>
		</li>
		<li><b>Cool stuff</b> - features that we have and others not
			<ul>
				<li><b>Automatic packing to texture atlaxes</b> - no more handwork, everything done automatically by tools</li>
				<li><b>Photoshop integration</b> - if you use photoshop you'll be happy, you can use PSD files to import sprites & animations </li>
			</ul>
		</li>
	</ul>
	

	\section tools Tools
	 <ul>
		<li><b>ResourcePacker</b> - tool that packs all your graphics resources to texture atlases automatically. 
			<ul>
				<li><b>Automatic packing to texture atlases</b> - no more handwork, everything done automatically by tools.</li>
				<li><b>Photoshop integration</b> - if your artists using photoshop they'll be happy, you can use PSD files to import sprites & animations.</li>
			</ul>	
		</li>
		<li><b>Font Gen</b> - tool to create graphics fonts. Works on Mac OS X only. </li>
	 </ul>

	\section install_sec Installation
	
	To use DAVA SDK you'll need the following: 
	<ul>
		<li>PC with Visual Studio 8.0+ or Mac OS X 10.6+ with latest XCode installed. </li>
		<li>Python</li>
	</ul>
 
	 \section sub_plans Future plans
	 
	 <b> Platforms</b>
	 <ul>
		 <li>We working on Android support and it will be available in nearest future. </li>
		 <li>In long term perspective we plan to add support of XBox 360, PSP NGP and Nintendo 3DS. </li>
		 <li>Do not ask about WP7. We do not plan to add it until release of their native SDK. </li>
	 </ul>
	 <b> Tools we working on </b>
	 <ul>
		 <li>UI Editor</li>
		 <li>Particle Editor</li>
	 </ul>
 */

/**
	\page page_getting_started Getting started / SDK Basics
	Recommended directory structure to avoid problems with multiple revisions of the framework:
	<p>
		Projects for each version of the framework should lie on the same level as framework itself. 
		Projects for v.1.0 of the framework.	
		<br/>
		~/user/projects/v10/dava.framework<br/>
		~/user/projects/v10/project1<br/>
		~/user/projects/v10/project2<br/>
		~/user/projects/v10/project3<br/>

		Projects for v.2.0 of the framework. 
		<br/>
		~/user/projects/v20/dava.framework<br/>
		~/user/projects/v20/project1<br/>
		~/user/projects/v20/project2<br/>
		~/user/projects/v20/project3<br/>
 
		<b>Why you should keep multiple revisions of the framework?</b> <br/>
		Normally it's not required. You can always switch to newest and you can get benifits from that. 
		But sometimes when your project is on final stage of development it's no make any sense to transfer it to new framework and pass whole 
		QA cycle again. So for such purposes you should keep all versions of the framework you actually need. 
 	</p>
	\section projdirstruct Project directory structure
	When you start your project you should think carefully about the directory structure.
	SDK propose you the following directory structure
	~/project_home/Classes - your source code for this specific project<br/>
	~/project_home/Win32Specific - specific code for win32 platform. Normally it contain only main.cpp<br/>
	~/project_home/MacOSSpecific - specific code for Mac OS X platform. Normally it contain only main.mm<br/>
	~/project_home/iOSSpecific - specific code for iOS platform. Normally it contain only main.mm, info.plist and MainWindow.xib files<br/>
	~/project_home/iPad - resources for iPad<br/>
	~/project_home/iPad/Data - compiled resources for iPad<br/>
	~/project_home/iPad/DataSource - compiled resources for iPad<br/>
	~/project_home/Win32 - resources for Win32<br/>
	~/project_home/Win32/Data - compiled resources for Win32<br/>
	~/project_home/Win32/DataSource - compiled resources for Win32<br/>
	~/project_home/MacOSX - resources for Mac OS X<br/>
	~/project_home/MacOSX/Data - compiled resources for Mac OS X<br/>
	~/project_home/MacOSX/DataSource - compiled resources for Mac OS X<br/>
 
	\section install_w32 Installation on Win32
	<ol>
		<li>Install Visual Studio 2005 or higher. We recommend Visual Studio 2010 Express Edition. </li>
		<li>Install Python 2.6+</li>
		<li>Install Open AL SDK 1.1</li>
	</ol>
 
	\section install_macos Installation on MacOS X
	<ol>
		<li>Install latest XCode. </li>
	</ol>
 
 
	<!-- 
	\subsection subsection1 The first subsection
	Text.
	\subsection subsection2 The second subsection
	More text.
	-->
 */

/**
	\page page_framework_architecture Application Architecture

	List of sections:
	- \ref how_to_start
	- \ref how_to_create
	- \ref ui_first_steps
	- \ref ui_event_handling
	- \ref firstgameobject
	
	\section how_to_start How to start development
	To start your own project you can copy Template Project and start writing the code. 
	
	Template project have the following files and folders:
	- Classes
		- FrameworkMain.cpp - functions that performed at framework launch and framework quit.
		- GameCore.cpp - implementation of ApplicationCore delegate 
		- GameCore.h - declaration of ApplicationCore delegate
		- FirstScreen.cpp - implementation of sample screen 
		- FirstScreen.h - declaration of sample screen
	- Data - directory for compiled resources (empty at the beginning)
	- DataSource - directory for resources that is not compiled 
		- convert_graphics.py - helper script to perform packing of graphics
		- Gfx
	
	Next section will describe how to create application manually and how FrameworkMain.cpp, GameCore.cpp and GameCore.h organized.  
 
	\section how_to_create How to create application
	To create your own application you need to perform the following steps:

	1. Create FrameworkMain.cpp and define FrameworkDidLaunched and FrameworkWillTerminate functions
	\code
	void FrameworkDidLaunched()
	{
		//	Create keyed archive for application options
		KeyedArchive * appOptions = new KeyedArchive();
		#if defined(__DAVAENGINE_IPHONE__)
		// set application base orientation (should be the same as in Info.plist)
		appOptions->SetInt("orientation", Core::SCREEN_ORIENTATION_PORTRAIT);

		// set virtual resolution you want to work in
		DAVA::Core::Instance()->SetVirtualScreenSize(320, 480);

		// register resources and their resolutions
		DAVA::Core::Instance()->RegisterAvailableResourceSize(320, 480, "Gfx");
		#else
		// set client area size for the windowed application
		appOptions->SetInt("width",	920);
		appOptions->SetInt("height", 690);

		appOptions->SetInt("fullscreen.width",	1024);
		appOptions->SetInt("fullscreen.height", 768);

		// this flag means that we start from windowed mode
		appOptions->SetInt("fullscreen", 0);

		// 32 bits per pixel
		appOptions->SetInt("bpp", 32); 

		DAVA::Core::Instance()->SetVirtualScreenSize(920, 690);
		DAVA::Core::Instance()->RegisterAvailableResourceSize(920, 690, "Gfx");
		#endif 

		// create our class that inherited from ApplicationCore
		GameCore * core = new GameCore();
		// set it as application core
		DAVA::Core::SetApplicationCore(core);
		// set launch options
		DAVA::Core::SetOptions(appOptions);
	}

	void FrameworkWillTerminate()
	{


	}
	\endcode

	2. Implement all virtual functions of ApplicationCore
	\code
	void GameCore::OnAppStarted()
	{
		RenderManager::Instance()->SetFPS(30);

		mainMenuScreen.Set(new MainMenuScreen());
		gameScreen.Set(new GameScreen());
		winScreen = newref(WinScreen);

		UIScreenManager::Instance()->RegisterScreen(SCREEN_MAIN_MENU, mainMenuScreen.Get());
		UIScreenManager::Instance()->RegisterScreen(SCREEN_GAME, gameScreen.Get());
		UIScreenManager::Instance()->RegisterScreen(SCREEN_WIN, winScreen.Get());

		UIScreenManager::Instance()->SetFirst(SCREEN_GAME);
	}

	void GameCore::OnAppFinished()
	{
	// Release RefPtr pointers to avoid memory leaks
		mainMenuScreen = 0;
		gameScreen = 0;
	}

	void GameCore::OnSuspend()
	{
	}

	void GameCore::OnResume()
	{
	}

	void GameCore::OnBackground()
	{
	}

	void GameCore::BeginFrame()
	{
		ApplicationCore::BeginFrame();
		RenderManager::Instance()->ClearWithColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void GameCore::Update(float32 timeElapsed)
	{	
		ApplicationCore::Update(timeElapsed);
	}

	void GameCore::Draw()
	{
		ApplicationCore::Draw();
	}
	\endcode
 
	\section ui_first_steps UI Introduction
 
	Here we'll describe how to create first UIScreen and create several controls inside. 
	
	\section ui_event_handling UI Event Handling 

	Here we'll describe how to handle events and use messages in our SDK.
	
	\section firstgameobject Game Objects Introduction
	In this section we'll introduce game objects and write you how to creeate manager to manage them and how to use it to manage couple of game objects.
	
	
 */
 
/**
	\page page_tutorial_packer Packer Tutorial

    \section sprites Sprites
    <p>
        To tell you about packer firstly I want to say few words about Sprites. 
        
        Sprite is low level rendering object that is used for biggest part of 2D graphics in our engine.
        Idea is pretty simple, Sprite is 2D image with different properties. 
        Our sprite has the following properties: position, scale, rotation, pivot point and frame.
        
        Example of sprite usage:
        \code
// in UIScreen::LoadResources
Sprite * sprite = Sprite::Create("~res:/Gfx/Menu/header");
 
// in Draw function
sprite->SetPosition(x, y);
sprite->SetAngle(DegToRad(30.0f));
sprite->Draw();
 
// in UIScreen::UnloadResources
SafeRelease(sprite);
        \endcode

        Sprites can have multiple frames. It can be very usefull in many situations. For the UI you can keep different states of the same element inside one sprite.
        In Game logic you can keep animations or different parts of the same object inside one sprite. 
        Typical cases of sprite usage: 2d animations, UI elements with same size but different states, complex game objects that have different sub-objects.

        To use Sprites you should prepare them first using ResourcePacker utility. It's available for Windows and MacOS X and written on SDK as well.
    </p>
    
    \section how_to How to use packer
    <p>
        After copying Framework template project you should have everything to start work with packer. 
        Each DataSource directory is equiped with script convert_graphics.py. This script allows you to run packer easily. 
        If you want to see all logs you should open terminal or console, find DataSource directory and run convert_graphics.py.
    </p>
    <p>
        convert_graphics.py automatically finds all subfolders with Gfx prefix inside DataSource directory and pack them to corresponding Data directory. 
        Corresponding Data directory lies on the same level with DataSource directory.
    </p>
    
    \section supported_formats Supported formats and packing rules
    <p>
        ResourcePacker support 2 formats: PNG and PSD. 
        
        Conversion rules for PSD: Packer unpack every frame of PSD file and use it as sprite frame. So if you have running character with all frames in PSD format after conversion 
        you'll have 1 sprite with all frames.
        
        Conversion rules for PNG: Packer open .pngdef file first and read number of frames in your sequence. According to the number of frames it split your PNG image and
        create 1 sprite with requested number of frames. Now supports only vertically packed images. 
        
    </p>
    \section how_it_works How ResourcePacker works
    <p>
        Let's say we have Game subfolder inside Gfx folder and 2 PSD sprites in this folder: soldier.psd and bullets.psd. 
        soldier.psd contain all frames required for soldier animation and bullets.psd contains all bullets for the game.
        
        1)First of all packer split every PSD file into frames.<br/>
        2)Then packer removes all unused space around each frame and try to pack them into 1 texture <br/>
        3)If it's possible to pack everything into one texture packer finish with this folder and moves to next folder <br/>
    
        Generally this is very simple explanation on how ResourcePacker works. 
    </p>

	\section convert_graphics convert_graphics.py
	
	This file is helper file for packer executable. This script use relative path to framework, so if script do not working you 
	should check is path is right.
 
	\code
# *************** HERE YOU SHOULD SETUP YOUR OWN PATHS ***************
# Darwin - MacOS Path
# Windows - Windows Path
framework_path = { "Darwin": "./../dava.framework", "Windows": "./../dava.framework" }
# *************** HERE YOU SHOULD SETUP YOUR OWN PATHS ***************
	\endcode
 
 
	\section packerflags Flags

	Every directory inside Gfx folder can contain flags.txt file. Using this file you can change packing options for this particular directory. 
	\code
--pvr - pack textures in this folder to pvr format
--tsize2048 - set max texture size to 2048
--add0pixel - set 0 pixels default space between sprite frames
--add1pixel - set 1 pixels default space between sprite frames
--add2pixel - set 2 pixels default space between sprite frames
--add3pixel - set 3 pixels default space between sprite frames
default value is 1
--debug - draw debug rects around frames
--split - every PSD in directory will be packed to own texture
--rebuild_this -- пересобирает только этот каталог
	\endcode
 
    <!-- 
	Notes:
        
        There are 3 main use cases:
        1) You 
        
        Sounds very simple but on practice there are few other options available.
        

    1) If your pack paths is different for example /DataSource/Gfx and /datasource/gfx packer will perform repack, so be carefull and use case-sentensive paths in all cases.
    Feature requests
    ==================================================
    1) Be first to write your request here.
	-->
 
 */
/**
	\page page_tutorial_particles Particle System in Action
	
	This tutorial will help you to use particle systems and get understanding of particle system parameters.
	It will be made on real examples to explain how to use these effects. 
	
	\section tutorial_particles_emitter Particle Emitters
	<p>
		\ref DAVA::ParticleEmitter is main class of our Particle Systems. Emitter do not generate particles but it defines 
		basic properties of particle system like position, angle, generation area, life time. So this is high-level 
		object and you can use it to animate position of your particle system. 
	</p>
	<p>
		Each particle emitter consist from multiple \ref DAVA::ParticleLayer's. Each ParticleLayer can generate particles and it actually 
		hold all information about particle generation like velocity, number of particles, frames, spin and many other things 
		like forces or motion randomness. Each particle layer limited by 1 sprite you can use for the generation. 
	</p>
 
	\section tutorial_particles_sparkles Simple sparkles
	Let's start from simple sparkles:
	\code
 
 
	\endcode
 
	\section tutorial_particles_adv_sparkles Advanced sparkles
	Here is example of advanced sparkles you can use in your project:
 
	
 */

