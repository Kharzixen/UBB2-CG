# DrimTim -- Computer Graphics Project


How to run: 

1) Download the project directory
2) Download the OGLPack: http://cg.elte.hu/~bsc_cg/resources/OGLPack.zip
3) Run the following command in the directory, where the OGLPack is located: "subst t: ."
4) In the project configuration (Right click -> Properties) set the Environment: Configuration Properties -> Debugging -> Environment PATH=t:\OGLPack\bin\x64;$PATH;
5) Check the VC++ Directories, the values should be:
Include Directories: T:\OGLPack\include;$(IncludePath)
Library Directories: t:\OGLPack\lib\x64;$(LibraryPath)