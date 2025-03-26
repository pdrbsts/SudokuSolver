import tkinter as tk
from tkinter import messagebox

class SudokuSolver:
    def __init__(self, root):
        self.root = root
        self.root.title("Sudoku Solver")

        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(padx=10, pady=10)

        self.entries = []
        vcmd = (root.register(self.validate_input), '%P', '%V', '%W')
        for i in range(9):
            row_entries = []
            for j in range(9):
                entry = tk.Entry(self.grid_frame, width=3, font=('Courier', 16),
                               justify='center', validate='key', validatecommand=vcmd,
                               relief=tk.SOLID, borderwidth=1)

                pad_left = 1
                pad_right = 1
                pad_top = 1
                pad_bottom = 1
                if j % 3 == 0:
                    pad_left = 3
                if j == 8:
                    pad_right = 3
                if i % 3 == 0:
                    pad_top = 3
                if i == 8:
                    pad_bottom = 3

                entry.grid(row=i, column=j, padx=(pad_left, 0), pady=(pad_top, 0), ipadx=3, ipady=3, sticky="nsew")

                entry.bind("<Up>", self.handle_arrow_keys)
                entry.bind("<Down>", self.handle_arrow_keys)
                entry.bind("<Left>", self.handle_arrow_keys)
                entry.bind("<Right>", self.handle_arrow_keys)
                entry.bind("<Return>", self.handle_arrow_keys)
                entry.bind("<BackSpace>", self.handle_backspace)

                row_entries.append(entry)
            self.entries.append(row_entries)

        for i in range(9):
            self.grid_frame.grid_rowconfigure(i, weight=1)
            self.grid_frame.grid_columnconfigure(i, weight=1)

        self.buttons_frame = tk.Frame(root)
        self.buttons_frame.pack(pady=10)

        self.solve_btn = tk.Button(self.buttons_frame, text="Solve", command=self.solve)
        self.solve_btn.pack(side=tk.LEFT, padx=5)

        self.clear_btn = tk.Button(self.buttons_frame, text="Clear", command=self.clear)
        self.clear_btn.pack(side=tk.LEFT, padx=5)

        if self.entries:
            self.entries[0][0].focus_set()

    def handle_arrow_keys(self, event):
        widget = event.widget
        try:
            current_row = widget.grid_info()['row']
            current_col = widget.grid_info()['column']
        except KeyError:
             return

        next_row, next_col = current_row, current_col

        if event.keysym == 'Up':
            next_row = max(0, current_row - 1)
        elif event.keysym == 'Down' or event.keysym == 'Return':
            next_row = min(8, current_row + 1)
        elif event.keysym == 'Left':
            next_col = max(0, current_col - 1)
        elif event.keysym == 'Right':
            next_col = min(8, current_col + 1)
        else:
            return

        if next_row != current_row or next_col != current_col:
            try:
                next_widget = self.entries[next_row][next_col]
                next_widget.focus_set()
                next_widget.select_range(0, tk.END)
            except IndexError:
                 pass
            return "break"
        else:
            pass

    def handle_backspace(self, event):
        widget = event.widget
        current_value = widget.get()
        cursor_pos = widget.index(tk.INSERT)

        if not current_value or cursor_pos == 0:
            try:
                current_row = widget.grid_info()['row']
                current_col = widget.grid_info()['column']
            except KeyError:
                return

            prev_col = current_col - 1
            prev_row = current_row

            if prev_col < 0:
                prev_col = 8
                prev_row -= 1

            if prev_row < 0:
                return

            prev_row = max(0, prev_row)

            try:
                prev_widget = self.entries[prev_row][prev_col]
                prev_widget.focus_set()
                prev_widget.select_range(0, tk.END)
                return "break"
            except IndexError:
                 pass

        return

    def validate_input(self, new_value, validation_type, widget_name):
        if validation_type == 'key':
            widget = self.root.nametowidget(widget_name)
            if new_value == "":
                return True
            try:
                num = int(new_value)
                if 1 <= num <= 9 and len(new_value) == 1:
                    try:
                        current_row = widget.grid_info()['row']
                        current_col = widget.grid_info()['column']
                        self.root.after_idle(lambda cr=current_row, cc=current_col: self.move_focus_after_input(cr, cc))
                    except (KeyError, IndexError):
                         pass
                    return True
                return False
            except ValueError:
                return False
        return True

    def move_focus_after_input(self, current_row, current_col):
        next_col = (current_col + 1) % 9
        next_row = current_row + (1 if next_col == 0 else 0)

        if next_row < 9:
            try:
                next_widget = self.entries[next_row][next_col]
                next_widget.focus_set()
                next_widget.select_range(0, tk.END)
            except IndexError:
                pass

    def solve(self):
        try:
            grid = []
            for i in range(9):
                row = []
                for j in range(9):
                    val = self.entries[i][j].get().strip()
                    row.append(int(val) if val else 0)
                grid.append(row)

            original_fixed = [[(grid[i][j] != 0) for j in range(9)] for i in range(9)]

            if self.solve_sudoku(grid):
                for i in range(9):
                    for j in range(9):
                        self.entries[i][j].config(fg='black')
                        if not original_fixed[i][j]:
                             self.entries[i][j].config(fg='blue')

                        self.entries[i][j].delete(0, tk.END)
                        self.entries[i][j].insert(0, str(grid[i][j]))
            else:
                messagebox.showerror("Error", "No solution exists for this puzzle")
        except ValueError:
            messagebox.showerror("Error", "Please enter valid numbers (1-9) or leave cells empty.")

    def clear(self):
        for i in range(9):
            for j in range(9):
                self.entries[i][j].delete(0, tk.END)
                self.entries[i][j].config(fg='black')
        if self.entries:
            self.entries[0][0].focus_set()

    def solve_sudoku(self, grid):
        empty = self.find_empty(grid)
        if not empty:
            return True
        row, col = empty
        for num in range(1, 10):
            if self.is_valid(grid, num, (row, col)):
                grid[row][col] = num
                if self.solve_sudoku(grid):
                    return True
                grid[row][col] = 0 # Backtrack
        return False

    def find_empty(self, grid):
        for i in range(9):
            for j in range(9):
                if grid[i][j] == 0:
                    return (i, j)
        return None

    def is_valid(self, grid, num, pos):
        row, col = pos
        for j in range(9):
            if grid[row][j] == num and j != col:
                return False
        for i in range(9):
            if grid[i][col] == num and i != row:
                return False
        box_row, box_col = row // 3, col // 3
        for i in range(box_row * 3, box_row * 3 + 3):
            for j in range(box_col * 3, box_col * 3 + 3):
                if grid[i][j] == num and (i, j) != pos:
                    return False
        return True


if __name__ == "__main__":
    root = tk.Tk()
    app = SudokuSolver(root)

    root.update_idletasks()
    window_width = root.winfo_width()
    window_height = root.winfo_height()
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    center_x = (screen_width // 2) - (window_width // 2)
    center_y = (screen_height // 2) - (window_height // 2)
    root.geometry(f'{window_width}x{window_height}+{center_x}+{center_y}')
    root.resizable(False, False)

    root.mainloop()
