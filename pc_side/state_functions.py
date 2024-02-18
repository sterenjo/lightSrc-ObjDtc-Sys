import time

import communication
import main
import utils

global state_label


def object_func():
    state_label.config(text="Current State: Mode changed to Object Detector System mode.\n")
    communication.send_msp(main.state_mapping['state_1'])

    # utils.get_obj_list()
    flag = True
    obj_list = []

    while flag:
        msb = communication.receive_byte_msp()
        lsb = communication.receive_byte_msp()
        deg = communication.receive_byte_msp()
        #val, deg = receive_obj_msp()
        communication.send_msp('1')
        val = msb*256 + lsb
        if val:
            obj_list.append((deg * 3, round((val / 58), 2)))
        else:
            flag = False

    utils.plot_half_circle(obj_list)


def telemeter_func():
    global state_label

    state_label.config(text="Current State: Mode changed to Telemeter mode.\n")
    communication.send_msp(main.state_mapping['state_2'])

    degree = utils.enter_degree_popup()
    utils.show_distance_popup(degree)


def ldr_func():

    state_label.config(text="Current State: Mode changed to Light Sources Detector System mode.\n")
    communication.send_msp(main.state_mapping['state_3'])

    # utils.get_ldr_list()
    measurments = [690.0, 696.0, 702.0, 708.0, 714.0, 720.0, 726.0, 732.0, 738.0, 744.0, 750.0, 760.0, 770.0, 780.0, 790.0, 800.0,
     802.0, 804.0, 806.0, 808.0, 810.0, 816.0, 822.0, 828.0, 834.0, 840.0, 852.0, 864.0, 876.0, 888.0, 900.0, 903.0,
     906.0, 909.0, 912.0, 915.0, 928.0, 941.0, 954.0, 967.0, 980.0, 984.0, 988.0, 992.0, 996.0, 1000]

    flag = True
    i=0
    ldr_list = []

    while flag:
        msb = communication.receive_byte_msp()
        lsb = communication.receive_byte_msp()
        deg = communication.receive_byte_msp()
        #val, deg = receive_obj_msp()
        communication.send_msp('1')
        val = msb*256 + lsb

        dist_cm = utils.find_closest_index(val,measurments)

        if val:
            ldr_list.append((deg * 3, dist_cm))
        else:
            flag = False

    utils.plot_half_circle(ldr_list)


def us_ldr_func():
    state_label.config(text="Mode changed to Light Sources and Objects Detector System mode.\n")
    communication.send_msp(main.state_mapping['state_4'])

    obj_list = utils.get_obj_list()
    ldr_list = utils.get_ldr_list()
    utils.plot_combined_half_circle(obj_list, ldr_list)

def download_script():
    state_label.config(text="Downloading Script\n")
    communication.send_msp(main.state_mapping['state_5'])
    utils.download_script_window()


def execute_script():
    state_label.config(text="Executing Script\n")
    communication.send_msp(main.state_mapping['state_6'])
    utils.execute_script_window()

