# Sudoku Solver GUI (Windows)

This is a simple graphical Sudoku solver application for Microsoft Windows, written in C using the native Win32 API.

## Description

The program presents a standard 9x9 Sudoku grid where users can input the initial numbers of a puzzle. It features:

*   A graphical user interface (GUI) with a 9x9 grid of input boxes.
*   Input validation: Only digits '1' through '9' and space are accepted in the cells.
*   Arrow key navigation between cells.
*   Automatic focus advancement to the next cell when a digit or space is entered.
*   Backspace functionality to clear a cell and move focus backward.
*   A "Solve" button that attempts to find a solution using a recursive backtracking algorithm.
    *   It first checks the initial user input for immediate conflicts (e.g., two identical numbers in the same row, column, or 3x3 block).
    *   If a solution is found, the grid is filled in. Numbers added by the solver (i.e., in cells that were initially empty) are displayed in **blue**.
    *   If the initial puzzle is invalid or has no solution, a message box informs the user.
*   A "Clear" button that resets the grid to an empty state.
*   Basic window management (centering, fixed size).

*(Optional: Add a screenshot of the running application here)*
<!-- ![Screenshot](screenshot.png) -->

## Requirements

*   **Operating System:** Microsoft Windows
*   **Compiler:** Microsoft Visual C++ (MSVC), which is part of:
    *   Visual Studio (Community Edition is free)
    *   Build Tools for Visual Studio (also free)
*   **Windows SDK:** Usually included with Visual Studio or the Build Tools.

## Compilation

You can compile the program using the Microsoft Visual C++ compiler (`cl.exe`) either directly from the command line or by using the provided `build.bat` script.

**Save the Code:** First, save the C code provided in the prompt into a file named `main.c`.

### Option 1: Using `build.bat` (Recommended Easy Way)

1.  Create a new text file in the *same directory* as `sudoku_solver.c`.
2.  Paste the following content into the text file:

    ```bat
    @echo off
    echo Building Sudoku.exe with MSVC...

    REM Set up the environment for MSVC (you may need to adjust the path)
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

    REM Compile resources
    rc.exe resources.rc

    REM Compile the program
    cl.exe /EHsc main.c resources.res /FeSudoku.exe

    echo Build completed successfully!
    pause
    ```

3.  Save the file as `build.bat`.
4.  Open a **Developer Command Prompt for Visual Studio** (you can find this in your Start Menu under the Visual Studio folder).
5.  Navigate (`cd`) to the directory where you saved `main.c` and `build.bat`.
6.  Run the batch script by typing `build.bat` and pressing Enter:

    ```cmd
    build.bat
    ```

7.  If successful, this will create an executable file named `Sudoku.exe` in the same directory.

### Option 2: Using MSVC Manually (Command Line)

1.  Open a **Developer Command Prompt for Visual Studio**.
2.  Navigate (`cd`) to the directory where you saved `main.c`.
3.  Execute the following command:

    ```cmd
    cl /W4 /EHsc main.c /Fe:Sudoku.exe user32.lib gdi32.lib comctl32.lib
    ```

    *   `cl`: Invokes the MSVC compiler/linker.
    *   `/W4`: Sets the warning level to 4 (a high level, good practice).
    *   `/EHsc`: Specifies the C++ exception handling model (standard).
    *   `main.c`: The input source file.
    *   `/Fe:Sudoku.exe`: Specifies the name for the output executable file.
    *   `user32.lib gdi32.lib comctl32.lib`: Specifies the necessary Windows libraries to link. (Note: The `#pragma comment(lib, ...)` directives in the code also tell the MSVC linker to use these libraries, so explicitly listing them might be redundant but is safe).

4.  If successful, this will create `Sudoku.exe`.

## How to Use

1.  Run the compiled `Sudoku.exe`.
2.  The application window will appear with an empty 9x9 grid.
3.  Click on a cell or use the arrow keys to navigate.
4.  Type the known digits ('1'-'9') of your Sudoku puzzle into the corresponding cells. Leave cells you want the solver to fill blank or press Space/Backspace to clear them.
5.  Once you have entered the puzzle numbers, click the **Solve** button.
6.  If a solution exists, the empty cells will be filled in. Numbers added by the solver will appear in blue text.
7.  If the puzzle you entered has conflicting numbers or no solution, a message box will appear.
8.  Click the **Clear** button to erase the entire grid and start over.
