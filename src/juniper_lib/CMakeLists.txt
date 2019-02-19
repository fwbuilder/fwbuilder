project(juniper_lib)

set(juniper_lib_srcs
    CompilerDriver_junosacl.cpp
    CompilerDriver_junosacl_run.cpp
    OSConfigurator_junos.cpp
    ../cisco_lib/PolicyCompiler_cisco.cpp
    ../cisco_lib/Helper.cpp
    PolicyCompiler_junosacl.cpp
    PolicyCompiler_junosacl_writers.cpp
    ../cisco_lib/NamedObjectsAndGroupsSupport.cpp
    ../cisco_lib/NamedObject.cpp
    ../cisco_lib/PolicyCompiler_cisco_acls.cpp
    ../cisco_lib/BaseObjectGroup.cpp
    ../cisco_lib/IOSObjectGroup.cpp
    ../cisco_lib/NamedObjectsManager.cpp
    ../cisco_lib/ACL.cpp
    ../cisco_lib/NXOSObjectGroup.cpp
    ../cisco_lib/PIXObjectGroup.cpp
    ../cisco_lib/ASA8ObjectGroup.cpp)

add_library(fwbjuniper STATIC ${juniper_lib_srcs})

target_include_directories(fwbjuniper PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(fwbjuniper fwbcisco compilerdriver)

target_compile_options(fwbjuniper PRIVATE ${CXX_DEFAULT_FLAGS})

qt5_use_modules(fwbjuniper Core)

