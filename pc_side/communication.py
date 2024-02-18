import time
import serial as ser

global s


def init_com(com_port):
    global s
    s = ser.Serial(com_port, baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)  # timeout of 1 sec so that the read and write operations are blocking,
    # after the timeout the program continues
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()


def send_msp(val):
    global s
    bytes_char = bytes(val, 'ascii')
    s.write(bytes_char)
    while s.out_waiting > 0:  # while the output buffer isn't empty
        continue


def send_str_msp(str_val):
    nextline = bytes('\n', 'ascii')
    for i in range(len(str_val)):
        time.sleep(0.1)
        bytes_str = bytes((str_val[i]), 'ascii')
        s.write(bytes_str)
    time.sleep(0.1)
    s.write(nextline)


def receive_byte_msp():
    while s.in_waiting == 0:
        continue
    val = int.from_bytes(s.read(), 'big')
    return val


def receive_obj_msp():
    while s.in_waiting < 2:
        time.sleep(0.01)
    msb = int.from_bytes(s.read(), 'big')
    lsb = int.from_bytes(s.read(), 'big')
    deg = int.from_bytes(s.read(), 'big')
    return msb * 256 + lsb, deg
