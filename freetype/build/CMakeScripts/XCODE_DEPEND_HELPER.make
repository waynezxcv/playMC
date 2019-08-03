# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.freetype.Debug:
/System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/Debug/libfreetyped.a:
	/bin/rm -f /System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/Debug/libfreetyped.a


PostBuild.freetype.Release:
/System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/Release/libfreetype.a:
	/bin/rm -f /System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/Release/libfreetype.a


PostBuild.freetype.MinSizeRel:
/System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/MinSizeRel/libfreetype.a:
	/bin/rm -f /System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/MinSizeRel/libfreetype.a


PostBuild.freetype.RelWithDebInfo:
/System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/RelWithDebInfo/libfreetype.a:
	/bin/rm -f /System/Volumes/Data/Users/liuwei/Desktop/GLL/freetype/build/RelWithDebInfo/libfreetype.a




# For each target create a dummy ruleso the target does not have to exist
