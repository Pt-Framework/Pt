Exit This platform is currently not supported with this toolset. Please edit the file "Jamrules.vc8" ;
ToolsetBinPath = [ FDirName $(ToolsetBinPath) x86_sh ] ;

LINKFLAGS += /machine:SH4 ;
C++FLAGS +=  /D "SHx" /D "_SHx_" ;
	