/*
    can only define tinyobj onces, can't do it in compiler because it will define it on every single file.
    also can't do it  in in the loadfromobj file because hten tehre would be an implementation in every file that includes it.
    need to do it here lolz. need to also include it that way the code for the tinyobj shit is added to atleast 1 .cpp file, because the define needs to happen before the 
    loader is included.
*/
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>