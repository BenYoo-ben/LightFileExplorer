## network_file_explorer
### [Build Manual](BuildManual.md)  
-----------------------------------------------------
### Source Code Diagram  
(powered by Sourcetrail)
![image](https://user-images.githubusercontent.com/57353430/179505518-d64e67be-1c35-49e3-b178-a0ef308bb20b.png)
_____________________________________________________
### Brief Manual
_____________________________________________________
  ### Server Process

```mermaid
graph TD
    A[START] --> |load config from cli or file|B(BINDING)
    C[global.hpp] --> |configs|B(binding)
    B --> |establish TCP server socket|D(TCP Socket Established)
    D --> |Start Loop|E(Main LOOP)
    E --> |listen & recv on TCP port|E
    E --> |on new client socket: + thread|F(Session Handler)
    F --> |parse recvd header|G(Call Handle Func)
    
    G --> AA(Download)
    G --> BB(Upload)
    G --> CC(Get Files Info)
    G --> DD(Copy, Move, Delete)
    
    AA --> |binary|T(TCP Stream)
    BB --> |binary|T
    CC --> |json|T
    DD --> |binary|T2(Internal File I/O)
```
  ### Request Protocol Format
  #### Over TCP Stack  
| Protocol Value              |    Field Name   | Protocol Type | Dir. Size            |      Directory       | Data Size               |        Data          |
| --------------------------- | --------------- |------------- | -------------------- | -------------------- | ----------------------- | -------------------- |
| FIELD SIZE>                 | -               |1 / byte      | 4 / uint32           | Dir. Size / string   | 4 / uint32              | Data Size / byte(s)  |
| 0:REQ_TYPE_DIR_INFO_DEPTH_2 | Get File Tree 2 |0             | DirSize in uint32    | DirName in string    | 0                       | -                    |
| 1:REQ_TYPE_DOWNLOAD_FILE    | Download File   |1             | DirSize in uint32    | DirName end with '/' | FileNameSize in uint32  | FileName with no path|
| 2:REQ_TYPE_COPY_FILE        | Copy File       |2             | SrcNameSize in uint32| SrcFileName FullPath | DstNameSize in uint32   | DstFileName FullPath |
| 3:REQ_TYPE_MOVE_FILE        | Move File       |3             | SrcNameSize in uint32| SrcFileName FullPath | DstNameSize in uint32   | DstFileName FullPath |
| 4:REQ_TYPE_DELETE_FILE      | Delete File     |4             | DirSize in uint32    | DirName end with '/' | FileNameSize in uint32  | FileName with no path|
| 5:REQ_TYPE_RENAME_FILE      | Rename File     |5             | SrcNameSize in uint32| SrcFileName FullPath | DstNameSize in uint32   | DstFileName FullPath |
| 6:REQ_TYPE_DIR_INFO_DEPTH_1 | Get Files in Dir|6             | DirSize in uint32    | DirName in string    | 0                       | -                    |
| 7:REQ_TYPE_UPLOAD_FILE      | Upload File     |7             | DirSize in uint32    | DirName in string    | FileNameSize in uint32  | FileName with no path|
  
    
  ### Session Handler: response scenario for each protocol type
#### Short Example
```
EX) Request for information in pi's home directory:  
Human) 6  |    8 bytes  | /  h  o  m  e  /  p  i  | 0           | -  
hex )  06 | 08 00 00 00 | 2F 68 6F 6D 65 2F 70 69 | 00 00 00 00 | -  
```

```
EX) Response from server example
DATA)
[
  {
    "auth" : "rwxr-xr-x",
    "is_dir" : "1",
    "name" : "test",
    "size" : "4096",
    "time" : "2021-10-18 10:46",
    "type" : ""
  },
  {
    "auth" : "rw-r--r--",
    "is_dir" : "",
    "name" : ".vimrc",
    "size" : "226",
    "time" : "2022-04-14 03:14",
    "type" : ""
  }
]
```
#### Data Formats  

**ACK Block**  
| Field | ACK Block |
| ----- | --------- |
| Size  | 4 bytes |
| all bytes = 0x00 |  
  
  
**Protocol: 0:REQ_TYPE_DIR_INFO_DEPTH_2, 6:REQ_TYPE_DIR_INFO_DEPTH_1**

```mermaid
sequenceDiagram
    Client->>Server: Send DIR_INFO request header

    Server->>Server: parser header and read DIR info
    
    Server->>Client: Data Block(json) (json string followed by strlen(uint32))
```  

**Protocol: 1:REQ_TYPE_DOWNLOAD_FILE**
```mermaid
sequenceDiagram
    Client->>Server: Send Download request header

    Server->>Server: parse header and start file send stream
    
    Server->>Client: Send File Size(uint32)
    
    Client->>Server: Send ACK Block
    
    Server->>Client: Send File over stream
    
    Client->>Client: Process stream into file
```  

**Protocol: 7:REQ_TYPE_UPLOAD_FILE**
```mermaid
sequenceDiagram
    Client->>Server: Send Upload request header
    
    Server->>Server: parse header and check availability
    
    Server->>Client: If available, send ACK block
    
    Client->>Server: Send file size in uint32
    
    Server->>Client: Read file size and prepare download stream
    
    Client->>Server: Send file data
    
    Server->>Server: Process stream into file    
```

**Protocol: 2:REQ_TYPE_COPY_FILE, 3:REQ_TYPE_MOVE_FILE, 4:REQ_TYPE_DELETE_FILE, 5:REQ_TYPE_RENAME_FILE**
```mermaid
sequenceDiagram
    Client->>Server: Send Protocol Header

    Server->>Server: parse header, process I/O ops.
    
    Server->>Client: ACK Block
```
  
  
