import subprocess
import serial
from othello_py import *
from random import choice
import tkinter


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

ai_player = 0

# 定数
offset_y = 10
offset_x = 10
rect_size = 60
circle_offset = 3

o = othello()
legal_buttons = []

# GUI部分
app = tkinter.Tk()
app.geometry('500x700')
app.title('Isevot')
canvas = tkinter.Canvas(app, width=500, height = 700)
pixel_virtual = tkinter.PhotoImage(width=1, height=1)

def on_closing():
    egaroucid.kill()
    app.destroy()

app.protocol("WM_DELETE_WINDOW", on_closing)

# 盤面の作成
for y in range(hw):
    for x in range(hw):
        canvas.create_rectangle(offset_x + rect_size * x, offset_y + rect_size * y, offset_x + rect_size * (x + 1), offset_y + rect_size * (y + 1), outline='black', width=2, fill='#16a085')

# 石数表示
stone_str = tkinter.StringVar()
stone_str.set('*● 2 - 2 ○ ')
stone_label = tkinter.Label(canvas, textvariable=stone_str, font=('', 50))
stone_label.place(x=250, y=600, anchor=tkinter.CENTER)

record = ''

def ai():
    global clicked, record
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
    cmds.append('400')
    cmds.append(str(o.player) + str(policy[1]) + str(policy[0]))
    for flip in flips:
        cmds.append(('3' if o.player == 0 else '2') + str(flip[1]) + str(flip[0]))
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
    stone_str.set(s)
    o.print_info()
    show_grid()

def get_coord(event):
    global clicked, record
    y = int(event.widget.cget('text')[0])
    x = int(event.widget.cget('text')[2])
    print(y, x)
    record += chr(ord('a') + y) + str(y)
    print(record)
    clicked = True
    o.move(y, x)
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
    stone_str.set(s)
    o.print_info()
    show_grid()

def show_grid():
    global clicked, legal_buttons
    for button in legal_buttons:
        button.place_forget()
    legal_buttons = []
    for y in range(hw):
        for x in range(hw):
            try:
                canvas.delete(str(y) + '_' + str(x))
            except:
                pass
            if o.grid[y][x] == vacant:
                continue
            color = ''
            if o.grid[y][x] == black:
                color = 'black'
            elif o.grid[y][x] == white:
                color = 'white'
            elif o.grid[y][x] == legal:
                end_flag = False
                if o.player != ai_player:
                    color = '#3498db'
                    legal_buttons.append(tkinter.Button(canvas, image=pixel_virtual, width=rect_size - circle_offset * 2, height=rect_size - circle_offset * 2, bg=color, text=str(y) + '_' + str(x)))
                    legal_buttons[-1].bind('<ButtonPress>', get_coord)
                    legal_buttons[-1].place(y=offset_y + rect_size * y, x=offset_x + rect_size * x)
                continue
            canvas.create_oval(offset_x + rect_size * x + circle_offset, offset_y + rect_size * y + circle_offset, offset_x + rect_size * (x + 1) - circle_offset, offset_y + rect_size * (y + 1) - circle_offset, outline='black', width=2, fill=color, tag=str(y) + '_' + str(x))
    if o.player == ai_player:
        app.after(10, ai)

canvas.place(y=0, x=0)
o.check_legal()
show_grid()
app.mainloop()