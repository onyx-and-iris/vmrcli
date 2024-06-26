# VMRCLI Command Line Utility

## `Tested against`

- Basic 1.1.1.1
- Banana 2.1.1.1
- Potato 3.1.1.1

## `Requirements`

- [Voicemeeter](https://voicemeeter.com/)

## `Use`

`./vmrcli.exe [-i] [-k] [-D] <api commands>`

Where:

- `i`: Enable interactive mode. If set any api commands passed will be ignored.
- `k`: The kind of Voicemeeter (basic, banana or potato). Use this to launch the GUI.
- `D`: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL

## `Script files`

Scripts can be loaded from text files, for example:

```powershell
./vbantxt-cli $(Get-Content .\example_commands.txt)
```
