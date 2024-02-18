import tkinter as tk
from tkinter import messagebox
import tkinter.simpledialog as simpledialog
import numpy as np
from matplotlib import pyplot as plt
import math

import communication
import main
import state_functions

global root

command_opcode = {
    'inc_lcd': '01',  # 1 arg
    'dec_lcd': '02',  # 1 arg
    'rra_lcd': '03',  # 1 arg
    'set_delay': '04',  # 1 arg
    'clear_lcd': '05',  # 0 args
    'servo_deg': '06',  # 1 arg
    'servo_scan': '07',  # 2 args
    'sleep': '08',  # 0 args
}

def print_menu():
    global root
    root = tk.Tk()
    state = 'state_0'
    root.title("Main Menu")
    root.geometry("700x600")  # Set the size of the root window
    # Set background color for the root window
    root.configure(bg="#f0f0f0")
    # Create buttons in the main window
    state_functions.state_label = tk.Label(root, text="Current State:\n" + state, font=("Segoe UI", 14, "bold"))
    state_functions.state_label.pack(pady=20)

    button_styles = {
        "font": ("Segoe UI", 10),
        "bg": "#0078D4",  # Blue color similar to Windows 11 theme
        "fg": "white",  # White text color for better visibility
        "relief": tk.GROOVE,  # Add a groove effect on button borders
        "activebackground": "#005A9E",  # Darker blue color when the button is clicked
        "activeforeground": "white",  # White text color when the button is clicked
        "padx": 40,  # Add padding to the buttons
        "pady": 10,
    }

    btn1 = tk.Button(root, text="Object Detector System", command=lambda: state_functions.object_func(),
                     **button_styles)
    btn1.pack(pady=10)

    btn2 = tk.Button(root, text="Telemeter", command=lambda: state_functions.telemeter_func(), **button_styles)
    btn2.pack(pady=10)

    btn3 = tk.Button(root, text="Light Sources Detector System", command=lambda: state_functions.ldr_func(),
                     **button_styles)
    btn3.pack(pady=10)

    btn4 = tk.Button(root, text="Light Sources and Objects Detector System",
                     command=lambda: state_functions.us_ldr_func(), **button_styles)
    btn4.pack(pady=10)

    btn5 = tk.Button(root, text="Download Script", command=lambda: state_functions.download_script(), **button_styles)
    btn5.pack(pady=10)

    btn6 = tk.Button(root, text="Execute Script Script", command=lambda: state_functions.execute_script(), **button_styles)
    btn6.pack(pady=10)

    btn7 = tk.Button(root, text="Exit", command=lambda: root.destroy(), **button_styles)
    btn7.pack(pady=10)

    # Start the main event loop
    root.mainloop()


def execute_script_window():
    execute_window = tk.Toplevel(root)
    execute_window.title("Executing Script")
    execute_window.geometry("400x275")  # Set the size of the root window
    # Set background color for the root window
    execute_window.configure(bg="#f0f0f0")

    button_styles = {
        "font": ("Segoe UI", 10),
        "bg": "#0078D4",  # Blue color similar to Windows 11 theme
        "fg": "white",  # White text color for better visibility
        "relief": tk.GROOVE,  # Add a groove effect on button borders
        "activebackground": "#005A9E",  # Darker blue color when the button is clicked
        "activeforeground": "white",  # White text color when the button is clicked
        "padx": 40,  # Add padding to the buttons
        "pady": 10,
    }

    # Create three buttons in the new window
    btn1 = tk.Button(execute_window, text="Script 1", command=lambda: on_script_click_execute('1', execute_window), **button_styles)
    btn1.pack(pady=10)

    btn2 = tk.Button(execute_window, text="Script 2", command=lambda: on_script_click_execute('2', execute_window), **button_styles)
    btn2.pack(pady=10)

    btn3 = tk.Button(execute_window, text="Script 3", command=lambda: on_script_click_execute('3', execute_window), **button_styles)
    btn3.pack(pady=10)

    back_btn = tk.Button(execute_window, text="back", command=lambda: execute_window.destroy(), **button_styles)
    back_btn.pack(pady=10)

def on_script_click_execute(script_name, window):
    communication.send_msp(script_name)
    val = '0'
    while val != '\n':
        val = communication.receive_byte_msp()
        if val == 54:
            msb_n = communication.receive_byte_msp()
            lsb_n = communication.receive_byte_msp()
            new_distance = round((msb_n * 256 + lsb_n) / 58, 2)
            print(new_distance)
        elif val == 55:
            state_functions.object_func()



def download_script_window():
    download_window = tk.Toplevel(root)
    download_window.title("Downloading Script")
    download_window.geometry("400x275")  # Set the size of the root window
    # Set background color for the root window
    download_window.configure(bg="#f0f0f0")

    button_styles = {
        "font": ("Segoe UI", 10),
        "bg": "#0078D4",  # Blue color similar to Windows 11 theme
        "fg": "white",  # White text color for better visibility
        "relief": tk.GROOVE,  # Add a groove effect on button borders
        "activebackground": "#005A9E",  # Darker blue color when the button is clicked
        "activeforeground": "white",  # White text color when the button is clicked
        "padx": 40,  # Add padding to the buttons
        "pady": 10,
    }

    # Create three buttons in the new window
    btn1 = tk.Button(download_window, text="Script 1", command=lambda: on_script_click_download("Script 1", download_window),
                     **button_styles)
    btn1.pack(pady=10)

    btn2 = tk.Button(download_window, text="Script 2", command=lambda: on_script_click_download("Script 2", download_window),
                     **button_styles)
    btn2.pack(pady=10)

    btn3 = tk.Button(download_window, text="Script 3", command=lambda: on_script_click_download("Script 3", download_window),
                     **button_styles)
    btn3.pack(pady=10)

    back_btn = tk.Button(download_window, text="Back", command=lambda: download_window.destroy(), **button_styles)
    back_btn.pack(pady=10)


def on_script_click_download(script_name, window):
    if script_name.startswith("Script"):
        communication.send_msp(script_name[-1])
        script_num = script_name[-1]
        file_name = f"script{script_num}.txt"
        acc = ""
        try:
            with open(file_name, "r") as f:
                for line in f:
                    parts = line.strip().split()
                    if parts:
                        command = parts[0]
                        arguments = parts[1:]
                        if len(arguments) != 0:
                            arguments = arguments[0].split(',')
                        arguments_hex = [format(int(arg), '02X') for arg in arguments]
                        parsed_line = parse_line(command, arguments_hex)
                        #for c in parsed_line:
                        acc += parsed_line
                            #print(c)

        except FileNotFoundError:
            print(f"File '{file_name}' not found.")
        communication.send_str_msp(acc)
        val = communication.receive_byte_msp()
        if val == 49:
            window.destroy()




def parse_line(command, arguments):
    opcode = command_opcode.get(command)

    if len(arguments) == 0:
        return f"{opcode}"

    if len(arguments) == 1:
        return f"{opcode}{arguments[0]}"

    if len(arguments) == 2:
        return f"{opcode}{arguments[0]}{arguments[1]}"

    return ""  # Invalid opcode


def plot_half_circle(obj_list):
    # Filter points within the range of 2cm to 450cm and within 0 to 180 degrees
    # filtered_points = [(deg, cm) for deg, cm in points_array if 0 <= deg <= 180 and 2 <= cm <= 450]

    if not obj_list:
        show_popup("No points to plot.")
        return

    # extract degrees and distance (cm) from the filtered points
    degrees, cms = zip(*obj_list)

    # conversion to radians
    radians = np.deg2rad(degrees)

    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    ax.set_xlim([-50, 50])
    ax.set_ylim([0, 50])
    semicircle = plt.Circle((0, 0), 50, color='gray', fill=False)
    ax.add_artist(semicircle)

    # add the detected objects as point on the graph
    x_points = cms * np.cos(radians)
    y_points = cms * np.sin(radians)
    ax.plot(x_points, y_points, 'bo')
    label_offset = 2
    label_fontsize = 7

    for i, txt in enumerate(obj_list):
        # if 0 <= txt[0] <= 180 and 2 <= txt[1] <= 450:
        ax.text(x_points[i], y_points[i] + label_offset, f"({txt[0]}°, {txt[1]} cm)",
                ha='center', va='bottom', fontsize=label_fontsize)

    ax.set_title("Detected objects by ultra-sonic", pad=20)
    ax.set_xlabel("X (cm)")
    ax.set_ylabel("Y (cm)")
    plt.show()


def find_closest_index(value, arr):  # TODO - is necessary?
    left = 0
    right = len(arr) - 1

    while left <= right:
        mid = (left + right) // 2

        if arr[mid] == value:
            return mid
        elif arr[mid] < value:
            left = mid + 1
        else:
            right = mid - 1

    # Now 'left' is the index of the closest value to the given 'value'
    if left == 0:
        return 0
    elif left == len(arr):
        return len(arr) - 1
    else:
        return left - 1 if abs(arr[left - 1] - value) < abs(arr[left] - value) else left


def enter_degree_popup():
    degree = tk.simpledialog.askinteger("Enter Degree", "Enter an integer degree between 0 and 160:")
    if degree is not None:
        communication.send_str_msp(str(degree))
        return str(degree)
    else:
        return 0

def show_distance_popup(degree):
    msb = communication.receive_byte_msp()
    lsb = communication.receive_byte_msp()
    #communication.send_msp('1')
    distance = round((msb * 256 + lsb) / 58, 2)

    def update_distance_label():
        msb_n = communication.receive_byte_msp()
        lsb_n = communication.receive_byte_msp()

        new_distance = round((msb_n * 256 + lsb_n) / 58, 2)
        if new_distance is not None and distance_popup.winfo_exists():
            #communication.send_msp('1')
            distance_label.config(text=f"The distance to the object at {degree}° is:\n {new_distance} cm.")
            distance_popup.after(100, update_distance_label)  # Schedule the next update after 100ms
        else:
            communication.send_msp(main.state_mapping['state_0'])

    def close_popup():
        if distance_popup.winfo_exists():
            distance_popup.grab_release()  # Release the grab so the main window is no longer blocked
            distance_popup.destroy()

    if distance is not None:
        distance_popup = tk.Toplevel()  # Create a new popup window
        distance_popup.title("Distance Information")
        distance_popup.geometry("300x150")
        distance_popup.grab_set()  # Grab the focus, so the main window is blocked until this popup is closed

        # Display the initial distance information in the popup window
        distance_label = tk.Label(distance_popup, text=f"The distance to the object at {degree}° is: {distance} cm.")
        distance_label.pack(pady=20)

        # Create a button to close the popup window
        close_button = tk.Button(distance_popup, text="Close", command=close_popup)
        close_button.pack(pady=10)

        # Add a protocol to handle the closing of the popup window using the 'x' button
        distance_popup.protocol("WM_DELETE_WINDOW", close_popup)

        # Schedule the initial update of the distance label
        distance_popup.after(10, update_distance_label)
    else:
        messagebox.showwarning("Distance Information", "No distance data received from MSP board.")

def get_obj_list():
    flag = True
    obj_list = []

    while flag:
        msb = communication.receive_byte_msp()
        lsb = communication.receive_byte_msp()
        deg = communication.receive_byte_msp()
        # val, deg = receive_obj_msp()
        communication.send_msp('1')
        val = msb * 256 + lsb
        if val:
            obj_list.append((deg * 3, round((val / 58), 2)))
        else:
            flag = False
    return obj_list

def get_ldr_list():
    measurments = [10.0, 78.0, 146.0, 214.0, 282.0, 350.0, 430.0, 510.0, 590.0,
                   670.0, 750.0, 760.0, 770.0, 780.0, 790.0, 800.0, 802.0, 804.0,
                   806.0, 808.0, 810.0, 816.0, 822.0, 828.0, 834.0, 840.0, 852.0,
                   864.0, 876.0, 888.0, 900.0, 903.0, 906.0, 909.0, 912.0, 915.0,
                   928.0, 941.0, 954.0, 967.0, 980.0, 984.0, 988.0, 992.0, 996.0, 1000]

    flag = True
    i = 0
    ldr_list = []

    while flag:
        msb = communication.receive_byte_msp()
        lsb = communication.receive_byte_msp()
        deg = communication.receive_byte_msp()
        # val, deg = receive_obj_msp()
        communication.send_msp('1')
        val = msb * 256 + lsb

        dist_cm = find_closest_index(val, measurments)

        if val:
            ldr_list.append((deg * 3, dist_cm))
        else:
            flag = False
    return ldr_list



def remove_closest_point(obj_list, ldr_list):
    def degree_difference(degree1, degree2):
        return abs(degree1 - degree2)

    for ldr_object in ldr_list:
        min_difference = float('inf')
        closest_point = None

        for obj_point in obj_list:
            obj_degree = obj_point[0]
            ldr_degree = ldr_object[0]
            diff = degree_difference(obj_degree, ldr_degree)

            if diff < min_difference:
                min_difference = diff
                closest_point = obj_point

        if closest_point:
            obj_list.remove(closest_point)

def plot_combined_half_circle(obj_list, ldr_list):
    # Filter points within the range of 2cm to 450cm and within 0 to 180 degrees
    # filtered_points = [(deg, cm) for deg, cm in points_array if 0 <= deg <= 180 and 2 <= cm <= 450]


    if not obj_list and not ldr_list:
        show_popup("No points to plot.")
        return

    if ldr_list:
        ldr_degrees, ldr_cms = zip(*ldr_list)
        remove_closest_point(obj_list, ldr_list)
    # extract degrees and distance (cm) from the filtered points
    if obj_list:
        obj_degrees, obj_cms = zip(*obj_list)

    # conversion to radians
    obj_radians = np.deg2rad(obj_degrees)
    ldr_radians = np.deg2rad(ldr_degrees)

    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    ax.set_xlim([-50, 50])
    ax.set_ylim([0, 50])
    semicircle = plt.Circle((0, 0), 50, color='gray', fill=False)
    ax.add_artist(semicircle)

    # add the detected objects as point on the graph
    obj_x_points = obj_cms * np.cos(obj_radians)
    obj_y_points = obj_cms * np.sin(obj_radians)
    ax.plot(obj_x_points, obj_y_points, 'bo')
    label_offset = 2
    label_fontsize = 7

    for i, txt1 in enumerate(obj_list):
        # if 0 <= txt[0] <= 180 and 2 <= txt[1] <= 450:
        ax.text(obj_x_points[i], obj_y_points[i] + label_offset, f"({txt1[0]}°, {txt1[1]} cm)",
                ha='center', va='bottom', fontsize=label_fontsize)

    # add the detected objects as point on the graph
    ldr_x_points = ldr_cms * np.cos(ldr_radians)
    ldr_y_points = ldr_cms * np.sin(ldr_radians)
    ax.plot(ldr_x_points, ldr_y_points, 'yo', color='#FFFF00')

    for i, txt2 in enumerate(ldr_list):
        # if 0 <= txt[0] <= 180 and 2 <= txt[1] <= 450:
        ax.text(ldr_x_points[i], ldr_y_points[i] + label_offset, f"({txt2[0]}°, {txt2[1]} cm)",
                ha='center', va='bottom', fontsize=label_fontsize)

    ax.set_title("Detected objects by ultra-sonic", pad=20)
    ax.set_xlabel("X (cm)")
    ax.set_ylabel("Y (cm)")
    plt.show()


def show_popup(message):
    messagebox.showinfo("Information", message)
