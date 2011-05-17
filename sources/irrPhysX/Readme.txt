==========================================================================
IrrPhysx SDK v0.2 For Irrlicht 1.5 And Physx by Nvidia 2.8.1
==========================================================================

  Content of this file:

  1. Directory Structure Overview
  2. How To Start
  3. Requirements
  4. Release Notes
  5. License
  6. Contact


==========================================================================
1. Directory Structure Overview
==========================================================================

  You will find the following directories in the SDK:
  
  \bin         	The compiled example applications, just start them to see
               	IrrPhysx in action.
  \doc		The CHM documentation of the SDK
  \examples    	Source code for the examples showing how to use the library.
  \include     	Header files to include when programming with the library.
  \lib         	Libs to link with your programs when using the library.
  \media       	Graphics resources for the example applications.
  \SRC 		The source code of IrrPhysx. This code is not needed to develop
	       	applications with the library, but it is included to enable
	       	recompilation and debugging, if necessary.    


==========================================================================
2. How to start
==========================================================================

  To see the library in action, just go to the \bin directory, and start
  some of the example applications.
  
  To start developing your own applications and games with the engine take 
  a look at the source code in the \examples directory.


==========================================================================
3. Requirements
==========================================================================

  Currently IrrPhysx is only provided under windows. It will hopefully later
  be extended to work under Linux and Mac.
  	

==========================================================================
4. Release Notes
==========================================================================

  Please note that the textures, 3D models and levels in the \media directory
  are copyright by their authors and not covered by the IrrPhysx license. 

  IrrPhysx is a library providing a wrapper for Physx by Nvidia for use with
  the Irrlicht graphics engine. The aim is to provide simple access to the
  power of Physx whilst completely abstracting Physx away from the user so
  they don't need to learn any of the functions and data types of Physx,
  rather they will be able to use the Irrlicht data types which they are used to.

  Currently IrrPhysx is still in development and it's possible that it will go
  through API breaking changes from version to version. At the moment it does
  not give access to all of the features of Physx but will allow the use of most
  of the basic features so that you can get going on development in the meantime
  until later features, which you may need, become available.


==========================================================================
5. License
==========================================================================

  The license of IrrPhysx is based on the zlib/libpng license.
  Even though this license does not require you to mention that you are
  using IrrPhysx in your product, an acknowledgement would be highly appreciated.

  You should note that using IrrPhysx means you must also follow the restrictions
  and requirements of the Physx licences. These can be found at this link:

    http://developer.nvidia.com/object/physx_EULA.html

  Please note that IrrPhysx is based in part on the work of the Independent
  JPEG Group, the zlib, and libpng (through the use of the Irrlicht Engine).
  This means that if you use IrrPhysx in your product, you must acknowledge
  somewhere in your documentation that you've used the IJG code and libpng.
  It would also be nice to mention that you use IrrPhysx and the zlib.
  See the README files in the jpeglib and the zlib directories of the
  Irrlicht Engine for further informations.


  The IrrPhysx License
  ===========================

  Copyright (C) 2008 Chris Mash

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.


==========================================================================
6. Contact
==========================================================================

  If you have problems, questions or suggestions, please do one of the following:

    Post on the IrrPhysx forum:
    http://www.freepowerboards.com/irrai/

    Visit the Irrlicht Engine's forum and post in the IrrPhysx thread:
    http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=30804  
    
    Or find more information and contact details at the IrrPhysx website:
    http://chris.j.mash.googlepages.com/irrphysx
