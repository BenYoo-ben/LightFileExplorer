# Network File Explorer  
![network_file_explorer_1](https://user-images.githubusercontent.com/57353430/168418387-ab4af734-770d-4e46-a37e-f137608fd05b.png)  
This program is a simple **Client <--> Server** file explorer based on TCP communications with brief GUI for client.

## Build
nana and jsoncpp libraries are included in the code but still some other libraries may not be installed in your system.    
If the build fails, pleases take a look at Makefile(s) build flags to check for the dependencies. 

Makefile(s) is used for build:
```
// Client  
$ cd client/  
$ make  
  
// Server  
$ cd server/  
$ make  
```
  
## Run
Take a look at variables inside  
```
client/includes/global.hpp  
server/includes/global.hpp
```
These files include port and IP address of the server to be used. 
When these information is modified, you need to rebuild programs too.    
For **Client & Server** do   
```
$ make clean && make  
```
  
After configurations are done do:  
  
```
// Start server  
$ ./fs_server  
  
// Server with root privileged files.
$ sudo ./fs_server  

// Currently there's no option for running process in background or as a daemon.
// So you would need to use Linux way of putting process in background for now.
// example )
$ ./fs_server > debug.log 2>&1 &
  
// Client  
$ ./fs_prober  
```
  
  
