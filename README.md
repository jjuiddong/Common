# Common Library

## How To Build

- Need Library
	- DirectXTK
	- Assimp
	- Boost 1.63
	- OpenCV 3.2
	- TBB 
- Property Manager Setting
	- Microsoft.Cpp.Win32.user
		- Include Directories
			- $(WindowsSdkDir_10)\include
			- boost_1_63_0
			- opencv\build\include
			- AssImp\include
			- assimp\build\include
			- tbb2017_20161128oss\include


		- Library Directories
			- boost_1_63_0\stage\lib
			- opencv\buildvs\lib\all
			- assimp\build\code\Release
			- tbb2017_20161128oss\lib\ia32\vc14_ui
			- $(WindowsSdkDir_10)\Lib
- DirectXTK add reference project
- Tool
  - Protocol Generator
    - need boost library rebuild
    - change STL debug level option for debug mode
    - ex) c:\tools\bjam toolset=msvc link=static threading=multi define="_ITERATOR_DEBUG_LEVEL=0" define="_HAS_ITERATOR_DEBUGGING=0" debug stage
   

