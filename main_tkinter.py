import subprocess
import serial
from othello_py import *

import tkinter as tk

serial_port = '/dev/ttyUSB0'
executable = '"./Egaroucid_for_Console/Egaroucid_for_Console.out" -level 5 -t 2 -quiet'



robot = serial.Serial(serial_port, 115200, timeout=1000000)
egaroucid = subprocess.Popen(executable, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, shell=True)

def send_cmds(cmds):
    for cmd in cmds:
        print(cmd)
        cmd += '\n'
        robot.write(cmd.encode())
        print(robot.read(4))
        response = robot.read()
        print(response)

o = othello()
o.check_legal()
record = ''



def next_move():
    global o, record
    grid_str = 'setboard '
    for yy in range(hw):
        for xx in range(hw):
            if o.grid[yy][xx] == black:
                grid_str += 'X'
            elif o.grid[yy][xx] == white:
                grid_str += 'O'
            else:
                grid_str += '-'
    grid_str += ' '
    grid_str += 'X' if o.player == black else 'O'
    grid_str += '\n'
    egaroucid.stdin.write(grid_str.encode('utf-8'))
    egaroucid.stdin.flush()
    egaroucid.stdin.write('go\n'.encode('utf-8'))
    egaroucid.stdin.flush()
    coord = egaroucid.stdout.readline().decode()
    print(coord)
    record += coord
    print(record)
    policy = [int(coord[1]) - 1, ord(coord[0]) - ord('a')]
    flips = o.flippable(policy[0], policy[1])
    cmds = []
    cmds.append('600')
    cmds.append(str(o.player) + str(policy[1]) + str(policy[0]))
    for flip in flips:
        cmds.append(('3' if o.player == 0 else '2') + str(flip[1]) + str(flip[0]))
    cmds.append(str(o.player + 4) + '00')
    print(cmds)
    send_cmds(cmds)
    o.move(policy[0], policy[1])
    print(policy)
    o.print_info()
    if not o.check_legal():
        o.player = 1 - o.player
        if not o.check_legal():
            o.print_info()
            egaroucid.kill()
            o.player = -1
            print('終局しました')
    s = ''
    if o.player == 0:
        s += '*'
    else:
        s += ' '
    s += '● '
    s += str(o.n_stones[0])
    s += ' - '
    s += str(o.n_stones[1])
    s += ' ○'
    if o.player == 1:
        s += '*'
    else:
        s += ' '
    o.print_info()

def reset():
    global o, record
    o = othello()
    o.check_legal()
    o.print_info()
    record = ''

def robot_reset():
    quit_cmds = ['000', '107']
    send_cmds(quit_cmds)

def exit_all():
    egaroucid.kill()
    quit_cmds = ['000', '107']
    send_cmds(quit_cmds)
    robot.close()
    exit()


root = tk.Tk()
root.title("Isevot")
root.geometry("500x400")

frame = tk.Frame(root)
frame.pack(fill = tk.BOTH, padx=20,pady=10)

BUTTON_WIDTH = 450
BUTTON_HEIGHT = 6

next_button = tk.Button(frame, text="next", command=next_move, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
next_button.pack()

reset_button = tk.Button(frame, text="reset", command=reset, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
reset_button.pack()

reset_button = tk.Button(frame, text="robot_reset", command=robot_reset, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
reset_button.pack()

exit_button = tk.Button(frame, text="exit", command=exit_all, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
exit_button.pack()

root.mainloop()
