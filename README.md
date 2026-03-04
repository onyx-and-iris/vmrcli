# Voicemeeter Remote Command Line Utility

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows-blue)](#requirements)

> A command-line interface for controlling Voicemeeter

## Compatibility

| Voicemeeter Version | Status |
|-------------------|--------|
| Basic 1.1.2.2 | ✅ Tested |
| Banana 2.1.2.2 | ✅ Tested |
| Potato 3.1.2.2 | ✅ Tested |

## Requirements

- **[Voicemeeter](https://voicemeeter.com/)** - Any version (Basic, Banana, or Potato)
- **Windows** operating system
- **Command line environment** (PowerShell, CMD, or Terminal)

## Usage

```powershell
.\vmrcli.exe [OPTIONS] <api_commands>
```

### Command Line Options

| Option | Description | Example |
|--------|-------------|----------|
| `-h` | Print help message | `vmrcli.exe -h` |
| `-v` | Show version information | `vmrcli.exe -v` |
| `-i` | Enable interactive mode | `vmrcli.exe -i` |
| `-I` | Interactive mode without prompt | `vmrcli.exe -I` |
| `-f` | Don't split input on spaces | `vmrcli.exe -f` |
| `-k <type>` | Launch Voicemeeter GUI | `-kbasic`, `-kbanana`, `-kpotato` |
| `-l <level>` | Set log level | `-lDEBUG`, `-lINFO`, `-lWARN` |
| `-e` | Enable extra console output | `vmrcli.exe -e` |
| `-c <path>` | Load user configuration | `-c "C:\config.txt"` |
| `-m` | Launch MacroButtons app | `vmrcli.exe -m` |
| `-s` | Launch StreamerView app | `vmrcli.exe -s` |

> **Note:** When using interactive mode (`-i`), command line API commands are ignored.

## `API Commands`

### Command Types

| Syntax | Action | Example |
|--------|---------|---------|
| `!command` | **Toggle** boolean values | `!strip[0].mute` |
| `command=value` | **Set** a parameter | `strip[0].gain=2.5` |
| `command+=value` | **Increment** a parameter | `bus[0].gain+=1.2` |
| `command-=value` | **Decrement** a parameter | `bus[0].gain-=3.8` |
| `command` | **Get** current value | `strip[0].label` |

> **Tip:** Use quotes around values containing spaces: `'strip[0].label="my device"'`

---

### Examples

#### **Basic Operations**
*Toggle mute, get values, and adjust gain*

```powershell
.\vmrcli.exe -kbasic -lINFO !strip[0].mute strip[0].mute bus[0].gain-=3.8
```

#### **Setting Labels with Spaces**
*Set labels and print them back*

```powershell
.\vmrcli.exe -kbanana -lDEBUG 'strip[0].label="my podmic"' strip[0].label
```

#### **Device Configuration**
*Configure hardware devices with complex names*

```powershell
.\vmrcli.exe -lDEBUG bus[2].mute=1 bus[2].mute 'bus[2].device.wdm="Realtek Digital Output (Realtek(R) Audio)"'
```

#### **Batch Operations**
*Multiple strip configurations in one command*

```powershell
.\vmrcli.exe `
  'strip[0].label="my podmic"' strip[0].label !strip[0].mute `
  'strip[1].label="my wavemic"' strip[1].label !strip[1].mute
```

### Quick Commands

*Convenient shortcuts for common Voicemeeter operations*

| Command | API Equivalent | Description |
|---------|----------------|-------------|
| `lock` | `command.lock=1` | 🔒 Lock Voicemeeter parameters |
| `unlock` | `command.lock=0` | 🔓 Unlock Voicemeeter parameters |
| `show` | `command.show=1` | 👁️ Show Voicemeeter interface |
| `hide` | `command.show=0` | 🙈 Hide Voicemeeter interface |
| `restart` | `command.restart=1` | 🔄 Restart Voicemeeter engine |

> **Available in both direct and interactive modes**

## Interactive Mode

*Real-time command interface for live audio control*

**Start interactive session:**
```powershell
.\vmrcli.exe -i
```

**Interactive prompt:**
```powershell
Interactive mode enabled. Enter 'Q' to exit.
>>
```

> **Important:** Command line API arguments are ignored when using `-i`

## Script Files

*Automate complex audio setups with script files*

### Loading Scripts

**From file content:**
```powershell
.\vmrcli.exe -lDEBUG $(Get-Content .\example_commands.txt)
```

**Via pipeline:**
```powershell
$(Get-Content .\example_commands.txt) | .\vmrcli.exe -lDEBUG -I
```

### Script Format Rules

| Feature | Syntax | Example |
|---------|--------|----------|
| **Multiple commands per line** | Space, `;`, or `,` separated | `strip[0].mute=1;bus[0].gain+=2` |
| **Comments** | Lines starting with `#` | `# This is a comment` |

## Build Instructions

*Compile from source using GNU Make*

### Prerequisites
- [GNU Make](https://www.gnu.org/software/make/) 
- GCC compiler (recommended)
- Windows development environment

### Build Commands

```bash
# Standard build
make

# Disable colored logging
make LOG_USE_COLOR=no

# Clean build artifacts
make clean
```

> **Pre-built binaries** are available in [Releases][releases] with coloured logging enabled

---

## Resources

### Official Documentation
- [Voicemeeter Remote C API][remoteapi-docs] - Complete API reference

### Acknowledgments
- **[rxi][rxi-user]** - Creator of the excellent [log.c][log-c] logging library

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

[releases]: https://github.com/onyx-and-iris/vmrcli/releases
[remoteapi-docs]: https://github.com/onyx-and-iris/Voicemeeter-SDK/blob/main/VoicemeeterRemoteAPI.pdf
[rxi-user]: https://github.com/rxi
[log-c]: https://github.com/rxi/log.c
