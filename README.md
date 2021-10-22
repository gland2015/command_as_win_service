# Description
 
 This project will generate a small windows service program that accept a command string. it execute the command when service start, and terminate command process when service stop.

# Example

Using powershell, run as Administrator, execute:

```powershell
sc create gland-service binPath= "C:\programfiles\win_services.exe --cmd node C:\programfiles\server.js --cwd C:\programfiles" start= auto displayname= gland-service
```

That will create windows service.

# Options

`--cmd`

    Your command that execute when windows service start.

`--cwd`

    Command work directory

Others is `windows sc.exe` options.

