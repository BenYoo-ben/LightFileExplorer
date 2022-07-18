# Network File Explorer  
(old client powered by nana library, now replaced by web client)
![network_file_explorer_1](https://user-images.githubusercontent.com/57353430/168418387-ab4af734-770d-4e46-a37e-f137608fd05b.png)  
This program is a simple **Client <--> Server** file explorer based on TCP communications with brief GUI for client.

----------

## Build  
*external library notice*  
> jsoncpp and google test is only external library used, I prepared external libraries in ext_libs dir.  
If libraries are not compatible with your system, you will have to get libraries for your system.

-----------

#### License Notice  
- jsoncpp is licensed under MIT license
- nana library is licensed under boost software license  
Details of the licenses are inside the ext_libs directory located inside server and client directories.  
Or you can visit below websites for details.  
[nana]
[jsoncpp]

-----------

Makefile(s) is used for build:
```
// debug(unoptimized) -> produces fs_server
$ make target
$ make

// release(optimized) -> produces fs_server.[date]
$ make release

// unittest
$ make test
```

------------

## Run
These files define global settings used in system.
```
client/includes/global.hpp  
server/includes/global.hpp
```
> These files include port and IP address of the server to be used by default(can be changed by input args)  
> When these information is modified, you need to rebuild programs too.   
  
After configurations are done do:  
  
```
// Start server  
$ ./fs_server  
$ ./fs_server [PORT TO USE]
  
// Server with root privileged files.
$ sudo ./fs_server  

// Currently there's no option for running process in background or as a daemon.
// So you would need to use Linux way of putting process in background for now.
// example )
$ ./fs_server > debug.log 2>&1 &
  
// Client  
$ ./fs_prober  
```
  
[jsoncpp]: https://github.com/open-source-parsers/jsoncpp
[nana]: http://nanapro.org/en-us/
