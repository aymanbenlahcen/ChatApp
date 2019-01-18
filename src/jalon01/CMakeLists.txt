cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON01)
find_package(Threads)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c seech.c clech.c)


add_executable(client ${SRC_CLIENT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
        server.c command.c chaine.c )


add_executable(server  ${SRC_SERVER})

target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT})
