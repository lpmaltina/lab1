"""Код для создания анимаций, иллюстрирующих движение точек.
Читает информацию о координатах точек из директорий input и output.
Сохраняет анимации в директорию animations."""
from typing import Dict, List
from matplotlib import pyplot as plt
from celluloid import Camera
from tqdm import tqdm

DELIMITER = ","

def read_input_data(filename: str) -> List[Dict[str, List[float]]]:
    """Функция для чтения входных данных.
    Читает содержимое файла с входными данными с именем filename.
    Возвращает список с одним элементом - словарём вида
    {"x": [x1, ..., xn], "y": [y1, ..., yn]}, где n - количество точек."""
    data = [{"x": [], "y": []}]
    with open(filename, "r", encoding="utf-8") as f:
        bodies = int(f.readline().strip())
        for _ in range(bodies):
            f.readline()  # масса
            x, y = f.readline().split()
            data[0]["x"].append(float(x))
            data[0]["y"].append(float(y))
            f.readline()  # скорость
    return data


def read_output_data(
        filename: str,
        data: List[Dict[str, List[float]]],
        bodies: int,
        time_steps: int
    ):
    """Функция для чтения выходных данных -
    коодинат точек на каждом временном шаге.
    Читает содержимое файла с выходными данными с именем filename.
    
    data - данные о первоначальном положении точек.
    Это список с одним элементом - словарём вида
    {"x": [x01, ..., x0n], "y": [y01, ..., y0n]}, где n - количество точек.

    Функция изменяет список data,
    добавляя данные о коодинатах точек в течение time_steps временных шагов.
    В результате data принимает вид:
    [
        {"x": [x01, ..., x0n], "y": [y01, ..., y0n]},
        {"x": [x11, ..., x1n], "y": [y11, ..., y1n]},
        ...
        {"x": [xt1, ..., xtn], "y": [yt1, ..., ytn]},
    ],
    где n - количество точек, t - количество временных шагов.
    """
    with open(filename, "r", encoding="utf-8") as f:
        f.readline()  # заголовок файла
        for _ in range(time_steps):
            line = f.readline()
            if line:
                _, *coords = line.split(DELIMITER)
                data.append({"x": [], "y": []})
                for j in range(bodies):
                    x = float(coords[j * 2])
                    y = float(coords[j * 2 + 1])
                    data[-1]["x"].append(x)
                    data[-1]["y"].append(y)


def create_animation(
        data: List[Dict[str, List[float]]],
        bodies: int,
        time_steps: int
    ):
    """Функция, которая создаёт анимацию движения точек
    на основе данных об их первоначальных координатах и
    координатах в течение time_steps временных шагов.
    
    data имеет вид:
    [
        {"x": [x01, ..., x0n], "y": [y01, ..., y0n]},
        {"x": [x11, ..., x1n], "y": [y11, ..., y1n]},
        ...
        {"x": [xt1, ..., xtn], "y": [yt1, ..., ytn]},
    ],
    где n - количество точек, t - количество временных шагов.

    Анимация сохраняется в директорию animations в файл с именем
    animation-<количество точек>-<количество временных шагов>.gif"""
    fig = plt.figure()
    camera = Camera(fig)
    xmin, xmax, ymin, ymax = plt.axis()
    delta_x = (xmax - xmin) / 10
    delta_y = (ymax - ymin) / 10

    for i in tqdm(range((time_steps + 1))):
        t = plt.scatter(data[i]["x"], data[i]["y"], color="blue")

        for j in range(bodies):
            x = data[i]["x"][j]
            y = data[i]["y"][j]
            plt.annotate(str(j), (x, y), xytext=(x + delta_x, y + delta_y))

        plt.legend([t], [f"time step {i}"])
        plt.title("Positions of Material Points")
        plt.xlabel("x")
        plt.ylabel("y")
        camera.snap()
    animation = camera.animate()
    animation_filename = f"animations/animation-{bodies}-{time_steps}.gif"
    animation.save(animation_filename, writer="pillow")


# Делаем визуализацию для параллельных программ с 8 потоками.
# Рассматриваемое количество точек: 64, 128.
# Рассматриваемое количество временных шагов: 10, 100, 1000.
if __name__ == "__main__":
    mode = "parallel"
    threads = 8
    for bodies in (64, 128):
        for time_steps in (10, 100, 1000):
            data = read_input_data(f"input/input-{bodies}.txt")
            read_output_data(
                f"output/output-{mode}-{threads}-{bodies}-{time_steps}.csv",
                data,
                bodies,
                time_steps
            )
            create_animation(data, bodies, time_steps)
