# _LiFE_
## _**Li**ght and **F**ast **F**ile **E**xplorer_
![image](https://user-images.githubusercontent.com/57353430/179503480-2fc3f378-3adc-4f52-8dd7-27bbdddd9039.png)
![image](https://user-images.githubusercontent.com/57353430/179503610-9679ef20-c699-4dff-87e1-583d8590973a.png)
___________________________________

```mermaid
sequenceDiagram
User ->> Web Server: Request File Operation
Web Server ->> File Server: REST Request -> Parsed Request
File Server ->> Web Server: Do Operation & Send resp
Web Server ->> User: Send Result
```
___________________________________
## *Many More Detailes of each Process*  
### [File Server Manual](file_server/README.md)  
### [Web Server Manual](web_server/README.md)  
____________________________________
