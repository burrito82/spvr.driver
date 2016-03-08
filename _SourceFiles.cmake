# $Id:$

set(RelativeDir ".")
set(RelativeSourceGroup "src")
set(SubDirs)

set(DirFiles
    smartvr.cpp
    smartvr.h
    openvr_driver.h

    _SourceFiles.cmake
)
set(DirFiles_SourceGroup "${RelativeSourceGroup}")

set(LocalSourceGroupFiles)
foreach(File ${DirFiles})
    list(APPEND LocalSourceGroupFiles "${RelativeDir}/${File}")
    list(APPEND ProjectSources "${RelativeDir}/${File}")
endforeach()
source_group(${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles})

set(SubDirFiles "")
foreach(Dir ${SubDirs})
    list(APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake")
endforeach()

foreach(SubDirFile ${SubDirFiles})
    include(${SubDirFile})
endforeach()

