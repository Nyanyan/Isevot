import subprocess
import serial
from othello_py import *
from random import choice


robot = serial.Serial('COM3',115200, timeout=1000000)
egaroucid = subprocess.Popen('egaroucid5.exe 10'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)

def send_cmds(cmds):
    for cmd in cmds:
        print(cmd)
        cmd += '\n'
        robot.write(cmd.encode())
        print(robot.read(3))
        response = robot.read()
        print(response)

o = othello()
o.check_legal()
record = ''

while True:
    grid_str = str(o.player) + '\n'
    for yy in range(hw):
        for xx in range(hw):
            if o.grid[yy][xx] == black:
                grid_str += '0'
            elif o.grid[yy][xx] == white:
                grid_str += '1'
            else:
                grid_str += '.'
        grid_str += '\n'
    #print(grid_str)
    egaroucid.stdin.write(grid_str.encode('utf-8'))
    egaroucid.stdin.flush()
    value, coord = egaroucid.stdout.readline().decode().split()
    print(value, coord)
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
            exit()
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
