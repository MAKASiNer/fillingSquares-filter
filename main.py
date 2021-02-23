# SCALE_STEP - кф. подобия фрагментов (отношение фрагмента к подфрагменту)
# для SCALE_STEP = 2 размер первого фрагмент будет Х, второй Х / 2, третьего Х / 2^2 и т.д.

# RECURSION_DEPTH - глубина рекурсии (максимальная вложенность фрагментов)
# для RECURSION_DEPTH = 5 рамер первого фрагмента будет 2^5

# BRIGHTNESS_LIMIT - начальный лимит яркости
# фрагмент со средней яркость большей лимита будет залит и обведен, иначе разобит на подфрагменты

# BRIGHTNESS_STEP - шаг изменения лимита (лимит для подфрагмента равен лимиту помноженому на шаг)
# для BRIGHTNESS_STEP = 0.8 лимит первого фрагмент будет Х, второго X * 0.8, третьего X * 0.8^2 и т.д.

# DRAW_FIRST_MESH - будет ли отрисована самая большая сетка

# MESH_COLOR - цвет сетки в формате (r, g, b)

# INPUT_IMAGE - путь до входного изображения

# OUTPUT_IMAGE - путь до выходного изображения



import sys
import datetime
import threading
from PIL import Image, ImageDraw



SCALE_STEP = 2
RECURSION_DEPTH = 7

BRIGHTNESS_LIMIT = 200
BRIGHTNESS_STEP = 0.8

DRAW_FIRST_MESH = False
MESH_COLOR = (153, 242, 218)

INPUT_IMAGE = "test.jpg"
OUTPUT_IMAGE = "result.png"



# функция обработки фрагмента
def render(start: list(), step: int, lim: int):
    if step <= 1: return
    # находим средниц цвет
    clr = 0.0
    for y in range(start[1], min(start[1] + step, img.size[1])):
        for x in range(start[0], min(start[0] + step, img.size[0])):
            clr += arr[x, y] / step ** 2
            
    # если перешли лимит, то заполняем
    if clr >= lim:      
        canvas.rectangle(
            (start[0], start[1], start[0] + step, start[1] + step),
            fill=(int(clr), int(clr), int(clr)),
            outline=MESH_COLOR)
        
    # иначе запускаем рекурсию
    else:
        # новый шаг и лимит 
        _step = int(step / SCALE_STEP)
        _lim = int(lim * BRIGHTNESS_STEP)
        
        # функция
        for y in range(start[1], start[1] + step, _step):
            for x in range(start[0], start[0] + step, _step):
                render((x, y), _step, _lim)



# глубина рекурсии  
if sys.getrecursionlimit() < 1000:
    sys.setrecursionlimit(1000)

# открываем и конвертим изображение в чб
img = Image.open(INPUT_IMAGE).convert("L")

# создаем выходное изображение
result = Image.new("RGB", img.size)
canvas = ImageDraw.Draw(result)

# загружаем пиксели в массив
arr = img.load()

# начальный шаг
step = SCALE_STEP ** RECURSION_DEPTH       

# цикл записывает координаты в список и отрисовывает первичную сетку
_list = list()
for y in range(0, img.size[1], step):
    for x in range(0, img.size[0], step):
        # первая сетка
        if DRAW_FIRST_MESH:
            canvas.rectangle(
                (x, y, x + step, y + step),
                outline=MESH_COLOR)
            
        # вносит коодинаты в список
        _list.append((x, y))
        

# запускает функцию 
start = datetime.datetime.now()
print("0.0% ", end='')
for i in range(len(_list)):
    # функция
    threading.Thread(target=render, args=(_list[i], step, BRIGHTNESS_LIMIT)).run()

    # индикация
    print("\r" * 5 + (str(i * 100 / len(_list))[0:4] + "%").ljust(5), end='')
print("\r" * 5 +"100.0%")
finish = datetime.datetime.now()

# вывод времени на обработку
print("runtime: " + str(finish - start))

# сохранение
result.save(OUTPUT_IMAGE)