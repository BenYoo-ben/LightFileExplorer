# _LiFE_
## _**Li**ght and **F**ast **F**ile **E**xplorer_
![image](https://user-images.githubusercontent.com/57353430/179510031-f75e617d-5da7-4baa-a524-d0422b5c5620.png)
![image](https://user-images.githubusercontent.com/57353430/179510180-55dbbff2-36f8-4393-9c34-e90080305c7d.png)
___________________________________
```mermaid
sequenceDiagram
User ->> Web Server: Request File Operation
Web Server ->> File Server: REST Request -> Parsed Request
File Server ->> Web Server: Do Operation & Send resp
Web Server ->> User: Send Result
```
___________________________________
## *Many More Detailes about each Process*  
### [File Server Manual](file_server/README.md)  
### [Web Server Manual](web_server/README.md)  
____________________________________
