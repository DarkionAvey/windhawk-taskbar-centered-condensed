import win32gui
import win32process
import psutil

def enum_handler(hwnd, result_list):
    try:
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        proc = psutil.Process(pid)
        exe_name = proc.name()
        class_name = win32gui.GetClassName(hwnd)
        result_list.append((class_name, exe_name))
    except Exception:
        pass  # skip processes we can't access

windows_info = []
win32gui.EnumWindows(enum_handler, windows_info)

# Remove duplicates and sort
unique_info = sorted(set(windows_info), key=lambda x: (x[1], x[0]))

for class_name, exe_name in unique_info:
    print(f"{class_name:30} -> {exe_name}")
