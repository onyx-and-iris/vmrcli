# Voicemeeter Remote Command Line Utility

## `Tested against`

- Basic 1.1.1.1
- Banana 2.1.1.1
- Potato 3.1.1.1

## `Requirements`

- [Voicemeeter](https://voicemeeter.com/)

## `Use`

```powershell
.\vmrcli.exe [-h] [-i|-I] [-k] [-D] [-v] [-c] [-m] [-s] <api commands>
```

Where:

- `h`: Prints the help message.
- `i`: Enable interactive mode, use (-I) to disable the '>>' prompt.
  - If set, any api commands passed on the command line will be ignored.
- `k`: The kind of Voicemeeter (basic, banana or potato). Use this to launch the GUI.
- `D`: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL
- `v`: Enable extra console output (toggle, set messages)
- `c`: Load a user configuration (give the full file path)
- `m`: Launch the MacroButtons application
- `s`: Launch the StreamerView application

## `API Commands`

- Commands starting with `!` will be toggled, use it with boolean parameters.
- Commands containing `=` will set a value. (use `+=` and `-=` to increment/decrement)
- All other commands with get a value.

Examples:

Launch basic GUI, set log level to INFO, Toggle Strip 0 Mute, print its new value, then decrease Bus 0 Gain by 3.8

```powershell
.\vmrcli.exe -kbasic -D2 !strip[0].mute strip[0].mute bus[0].gain-=3.8
```

Launch banana GUI, set log level to DEBUG, set Strip 0 label to podmic then print Strip 2 label

```powershell
.\vmrcli.exe -kbanana -D1 strip[0].label=podmic strip[2].label
```

#### `Quick Commands`

A short list of quick commands are available:

- `lock`: command.lock=1
- `unlock`: command.lock=0
- `show`: command.show=1
- `hide`: command.show=0
- `restart`: command.restart=1

They may be used in direct or interactive mode.

## `Interactive Mode`

Running the following command in Powershell:

```powershell
.\vmrcli.exe -i
```

Will open an interactive prompt:

```powershell
Interactive mode enabled. Enter 'Q' to exit.
>>
```

API commands follow the same rules as listed above. Entering `Q` or `q` will exit the program.

## `Script files`

Scripts can be loaded from text files, for example in Powershell:

```powershell
.\vmrcli.exe -D1 $(Get-Content .\example_commands.txt)
```

You may also pipe a scripts contents to the CLI:

```powershell
$(Get-Content .\example_commands.txt) | .\vmrcli.exe -D1 -I
```

Multiple API commands can be in a single line, they may be separated by space, `;` or `,`.

Lines starting with `#` will be interpreted as comments.

## `Build`

Run the included `makefile` with [GNU Make](https://www.gnu.org/software/make/).

The binary in [Releases][releases] is compiled with coloured logging enabled. To disable this you can override the `LOG_USE_COLOR` variable, for example:

`make LOG_USE_COLOR=no`

## `Official Documentation`

- [Voicemeeter Remote C API][remoteapi-docs]

## `Special Thanks`

- [rxi][rxi-user] for writing the [log.c][log-c] package

[releases]: https://github.com/onyx-and-iris/vmrcli/releases
[remoteapi-docs]: https://github.com/onyx-and-iris/Voicemeeter-SDK/blob/main/VoicemeeterRemoteAPI.pdf
[rxi-user]: https://github.com/rxi
[log-c]: https://github.com/rxi/log.c
