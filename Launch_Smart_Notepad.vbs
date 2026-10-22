Set WshShell = CreateObject("WScript.Shell")
WshShell.Run "cmd /c start """" """ & Replace(WScript.ScriptFullName, "Launch_Smart_Notepad.vbs", "index.html") & """", 0, False
