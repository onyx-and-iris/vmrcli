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

- `i`: Enable interactive mode. If set, any api commands passed on the command line will be ignored.
- `k`: The kind of Voicemeeter (basic, banana or potato). Use this to launch the GUI.
- `D`: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL

## `API Commands`

- Commands starting with `!` will be toggled, use it with boolean parameters.
- Commands containing `=` will set a value.
- All other commands with get a value.

Examples:

Launch basic GUI, set log level to INFO, Toggle Strip 0 Mute, then print its new value

`./vmrcli.exe -kbasic -D2 !strip[0].mute strip[0].mute`

Launch banana GUI, set log level to DEBUG, set Strip 0 label to podmic then print Strip 2 label

`./vmrcli.exe -kbanana -D1 strip[0].label=podmic strip[2].label`

## `Interactive Mode`

Running the following command in Powershell:

`./vmrcli.exe -i`

Will open an interactive prompt:

```powershell
Interactive mode enabled. Enter 'Q' to exit.
>>
```

API commands follow the same rules as listed above. Entering `Q` or `q` will exit the program.

## `Script files`

Scripts can be loaded from text files, for example in Powershell:

```powershell
./vmrcli.exe -D1 $(Get-Content .\example_commands.txt)
```

## `Official Documentation`

- [Voicemeeter Remote C API](https://github.com/onyx-and-iris/Voicemeeter-SDK/blob/main/VoicemeeterRemoteAPI.pdf)

## `Special Thanks`

- [rxi](https://github.com/rxi) for writing the [log.c](https://github.com/rxi/log.c) package
