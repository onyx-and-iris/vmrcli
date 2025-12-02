# Voicemeeter Remote Command Line Utility

## `Tested against`

- Basic 1.1.1.9
- Banana 2.1.1.9
- Potato 3.1.1.9

## `Requirements`

- [Voicemeeter](https://voicemeeter.com/)

## `Use`

```powershell
.\vmrcli.exe [-h] [-v] [-i|-I] [-f] [-k] [-l] [-e] [-c] [-m] [-s] <api commands>
```

Where:

- `h`: Print the help message.
- `v`: Print the version of vmrcli.
- `i`: Enable interactive mode, use (-I) to disable the '>>' prompt.
  - If set, any api commands passed on the command line will be ignored.
- `f`: Do not split input on spaces.
- `k`: The kind of Voicemeeter (basic, banana or potato). Use this to launch the GUI.
- `l`: Set log level, must be one of TRACE, DEBUG, INFO, WARN, ERROR, or FATAL
- `e`: Enable extra console output (toggle, set messages)
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
.\vmrcli.exe -kbasic -lINFO !strip[0].mute strip[0].mute bus[0].gain-=3.8
```

Launch banana GUI, set log level to DEBUG, set Strip 0 label to podmic then print Strip 2 label

```powershell
.\vmrcli.exe -kbanana -lDEBUG strip[0].label=podmic strip[2].label
```

#### `String Commands With Spaces`

It may be desirable to send a string request containing spaces, for example to change an output device. By default the CLI splits such strings, to avoid this pass the `-f` flag. It's probably best to use this with single commands only due to its effect on how the CLI parses strings. Also note the inclusion of the double quotation marks, it seems the C API requires them.

```powershell
.\vmrcli.exe -lDEBUG -f bus[1].device.wdm='"Realtek Digital Output (Realtek(R) Audio)"'

.\vmrcli.exe -lDEBUG -f strip[0].label='"My Podmic"'
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
.\vmrcli.exe -lDEBUG $(Get-Content .\example_commands.txt)
```

You may also pipe a scripts contents to the CLI:

```powershell
$(Get-Content .\example_commands.txt) | .\vmrcli.exe -lDEBUG -I
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
