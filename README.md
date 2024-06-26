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

## `API Commands`

- Commands starting with `!` will be toggled, use it with boolean parameters.
- Commands containing `=` will set a value.
- All other commands with get a value.

Examples:

Launch basic GUI, set debug level to INFO, Toggle Strip 0 Mute, then print its new value

`./vmrcli.exe -kbasic -D2 !strip[0].mute strip[0].mute`

Launch banana GUI, set debug level to DEBUG, set Strip 0 label to podmic then print Strip 2 label

`vmrcli.exe -kbanana -D1 strip[0].label=podmic strip[2].label`

## `Script files`

Scripts can be loaded from text files, for example:

```powershell
./vbantxt-cli $(Get-Content .\example_commands.txt)
```