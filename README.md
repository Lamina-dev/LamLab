<<<<<<< HEAD
# LaminaLab
最近更新: parent of a84a2c4... 更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets
=======
# LaminaLab IDE v0.0.1-Alpha

A lightweight IDE specifically designed for the Lamina programming language, built with wxWidgets and powered by Scintilla text editor.

## System Requirements

- **Tested Operating System**: Windows 10/11 (x64)
- **Tested Compiler**: Visual Studio 2022 with MSVC 19.44+
- **Tested Build System**: CMake 3.16 or higher
- **Tested Package Manager**: vcpkg (for dependencies)

## Development Environment Setup

### 1. Prerequisites Installation

```bash
# Install Visual Studio 2022 Community (free)
# Download from: https://visualstudio.microsoft.com/vs/

# Install CMake
# Download from: https://cmake.org/download/

# Install Git
# Download from: https://git-scm.com/download/win
```

### 2. vcpkg Setup

```bash
# Clone vcpkg (if not already installed)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio
.\vcpkg.exe integrate install

# Set environment variable
setx VCPKG_ROOT "C:\vcpkg"
```

### 3. Install Dependencies

```bash
# Navigate to project directory
cd C:\path\to\LamLab

# Install wxWidgets (this will take some time)
vcpkg install wxwidgets:x64-windows
```

### 4. Build the Project

```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build the project
cmake --build . --config Debug

# Or for Release build
cmake --build . --config Release
```

### 5. Run the IDE

```bash
# From project root directory
.\build\bin\Debug\LaminaIDE.exe

# Or for Release build
.\build\bin\Release\LaminaIDE.exe
```

## Keyboard Shortcuts

### File Operations
- `Ctrl+N` - New file
- `Ctrl+O` - Open file
- `Ctrl+S` - Save file
- `Ctrl+Shift+S` - Save As
- `Alt+F4` - Exit application

### Edit Operations
- `Ctrl+Z` - Undo
- `Ctrl+Y` - Redo
- `Ctrl+X` - Cut
- `Ctrl+C` - Copy
- `Ctrl+V` - Paste
- `Ctrl+A` - Select All
- `Ctrl+F` - Find text

### Script Execution
- `F5` - Run script
- `Shift+F5` - Stop script execution

## Configuration

### Interpreter Setup

1. Go to **Run** → **Interpreter Path...**
2. Configure the path to your LaminaLab interpreter
3. Use `%lmfilepath%` as placeholder for the current file path
4. Default setting: `./laminalab %lmfilepath%`

### Example Configuration
```
# If LaminaLab is in your PATH
laminalab %lmfilepath%

# If using relative path
./laminalab %lmfilepath%

# If using absolute path
C:\path\to\laminalab.exe %lmfilepath%

# With additional arguments
laminalab --verbose %lmfilepath%
```

## Project Structure

```
LamLab/
├── build/                  # Build output directory
├── data/                   # Sample data files
├── include/                # Header files
│   ├── LaminaApp.h
│   ├── MainFrame.h
│   ├── LaminaEditor.h
│   └── ProcessManager.h
├── src/                    # Source files
│   ├── main.cpp
│   ├── LaminaApp.cpp
│   ├── MainFrame.cpp
│   ├── LaminaEditor.cpp
│   └── ProcessManager.cpp
├── third_party/            # Third-party dependencies
├── CMakeLists.txt          # CMake configuration
└── README.md              # This file
```

## IDE Layout

```
┌─────────────────────────────────────────┐
│ File  Edit  Run  Help              [×] │ ← Menu Bar
├─────────────────────────────────────────┤
│ [New] [Open] [Save] │ [Cut] [Copy] [Run]│ ← Tool Bar
├─────────────────────────────────────────┤
│                                         │
│           Code Editor                   │ ← Main Editor
│        (Syntax Highlighting)           │
│                                         │
├─────────────────────────────────────────┤
│           Console Output                │ ← Output Panel
│     (Script execution results)         │
├─────────────────────────────────────────┤
│ Ready │ Line 1, Column 1 │ Status      │ ← Status Bar
└─────────────────────────────────────────┘
```

## Getting Started

1. **Start the IDE**: Run `LaminaIDE.exe`
2. **Configure Interpreter**: Set up LaminaLab interpreter path
3. **Create New File**: `Ctrl+N` or File → New
4. **Write Code**: Use the syntax-highlighted editor
5. **Run Script**: Press `F5` to execute
6. **View Output**: Check the console panel for results

## Troubleshooting

### Common Issues

**Build Errors:**
- Ensure vcpkg is properly integrated: `vcpkg integrate install`
- Check that VCPKG_ROOT environment variable is set
- Verify Visual Studio 2022 with C++ workload is installed

**Runtime Issues:**
- Ensure all DLL dependencies are in PATH
- Check that interpreter path is correctly configured
- Verify .lm file permissions and accessibility

**Performance Issues:**
- Use Release build for better performance
- Check available system memory
- Close unnecessary applications

## File Associations

The IDE works with Lamina language files:
- **Extension**: `.lm`
- **MIME Type**: `text/x-lamina`
- **Description**: Lamina Source File

## Updates

Settings are automatically saved to Windows Registry under:
```
HKEY_CURRENT_USER\Software\LaminaLabIDE
```

This includes:
- Window position and size
- Interpreter configuration
- Recent files (planned)

## Support

For issues, suggestions, or contributions, please visit the project repository or contact the development team.

---

**LaminaLab IDE v0.0.1-Alpha** - Built with ❤️ for the Lamina programming language community.
>>>>>>> a84a2c4 (更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets)
