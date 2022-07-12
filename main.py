import subprocess
import serial
from othello_py import *
from random import choice


robot = serial.Serial('COM10',115200, timeout=1000000)
egaroucid = subprocess.Popen('egaroucid5.exe 10'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)

def send_cmds(cmds):
    for cmd in cmds:
        print(cmd)
        cmd += '\n'
        robot.write(cmd.encode())
        print(robot.read(3))
        response = robot.read()
        print(response)

''' demo
for cmd in ['400', '023', '333']:
    print(cmd)
    cmd += '\n'
    robot.write(cmd.encode())
    print(robot.read(3))
    response = robot.read()
    print(response)
'''


o = othello()
while True:
    if not o.check_legal():
        o.player = 1 - o.player
        if not o.check_legal():
            break
    '''
    legal_list = []
    for y in range(hw):
        for x in range(hw):
            if o.grid[y][x] == legal:
                legal_list.append([y, x])
    policy = choice(legal_list)
    '''
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
    policy = [int(coord[1]) - 1, ord(coord[0]) - ord('a')]
    flips = o.flippable(policy[0], policy[1])
    cmds = []
    cmds.append('400')
    cmds.append(str(o.player) + str(policy[1]) + str(policy[0]))
    for flip in flips:
        cmds.append(('3' if o.player == 0 else '2') + str(flip[1]) + str(flip[0]))
    print(cmds)
    send_cmds(cmds)
    o.move(policy[0], policy[1])
    print(policy)
    o.print_info()