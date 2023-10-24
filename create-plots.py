"""Код для построения графиков ускорения (speedup) и эффективности.
Графики строятся на основе данных о времени выполнения вычислений
из файла timings.csv и сохраняются в директорию plots."""
from typing import Dict
import matplotlib.pyplot as plt

N_BODIES = (64, 128, 256, 512, 1024)
N_TIME_STEPS = (10, 100, 1000)
DELIMITER = ","


def read_timings(filename: str) -> Dict[str, Dict[str, Dict[int, float]]]:
    """Функция, которая читает из файла с именем filename время вычислений.
    
    Возвращает словарь с информацией о времени вычислений.
    Ключи словаря - строки вида <количество точек>-<количество временных шагов>,
    значения словаря - словари.
    Ключ этого вложенного словаря - "time", значение по этому ключу - словарь.
    Ключи этого словаря - количество потоков,
    значения - время вычислений для данного количества точек,
    временных шагов и потоков."""
    timings = {}
    with open(filename, "r", encoding="utf-8") as f:
        f.readline()  # заголовок файла
        for line in f:
            line = line.strip()
            if line:
                threads, bodies, time_steps, time = line.split(DELIMITER)
                threads = int(threads)
                bodies = int(bodies)
                time_steps = int(time_steps)
                time = float(time)
                key = f"{bodies}-{time_steps}"
                if key in timings:
                    timings[key]["time"][threads] = time
                else:
                    timings[key] = {"time": {threads: time}}
    return timings


def count_speedup_and_efficiency(
        timings: Dict[str, Dict[str, Dict[int, float]]]
        ):
    """Функция для вычисления ускорения (speedup) и эффективности.
    
    timings - словарь с информацией о времени вычислений.
    Ключи словаря - строки вида <количество точек>-<количество временных шагов>,
    значения словаря - словари.
    Ключ этого вложенного словаря - "time", значение по этому ключу - словарь.
    Ключи этого словаря - количество потоков,
    значения - время вычислений для данного количества точек,
    временных шагов и потоков.
    
    Функция обновляет словарь timings,
    добавляя в словарь с ключом "time" ключи "speedup" и "efficiency".
    По ключу "speedup" значением будет словарь.
    Ключи этого словаря - количество потоков,
    значения - ускорение (speedup) для данного количества точек,
    временных шагов и потоков.
    По ключу "efficiency" значением будет словарь.
    Ключи этого словаря - количество потоков,
    значения - эффективность для данного количества точек,
    временных шагов и потоков."""
    for bodies in N_BODIES:
        for time_steps in N_TIME_STEPS:
            key = f"{bodies}-{time_steps}"
            one_thread_time = timings[key]["time"][1]
            timings[key]["speedup"] = {}
            timings[key]["efficiency"] = {}
            for threads, info in timings[key]["time"].items():
                timings[key]["speedup"][threads] = one_thread_time / info
                timings[key]["efficiency"][threads] = (
                    timings[key]["speedup"][threads] / threads
                )


def create_plot(
        timings: Dict[str, Dict[str, Dict[int, float]]],
        feature: str
    ):
    """Функция для построения графиков.
    timings - словарь с информацией о времени вычислений.
    Ключи словаря - строки вида <количество точек>-<количество временных шагов>,
    значения словаря - словари.
    Ключи этого вложенного словаря - "time", "speedup", "efficiency",
    значение по этому ключу - словарь.
    Ключи этого словаря - количество потоков,
    значения - время вычислений ИЛИ ускорение (speedup) ИЛИ эффективность
    для данного количества точек, временных шагов и потоков.
    
    Возможные значения параметра feature - "speedup" или "efficiency".
    График сохраняется в директорию plots."""
    _, axs = plt.subplots(
        1,
        len(N_TIME_STEPS),
        figsize=(5 * len(N_TIME_STEPS), 5)
    )
    plt.suptitle(feature.title())
    for bodies in N_BODIES:
        for i, time_steps in enumerate(N_TIME_STEPS):
            key = f"{bodies}-{time_steps}"
            label = f"{bodies} bodies"
            axs[i].set_title(f"{time_steps} time steps")
            axs[i].plot(
                timings[key][feature].keys(),
                timings[key][feature].values(),
                label=label
            )
    plt.legend()
    plt.savefig(f"plots/{feature}.png")


timings = read_timings("timings.csv")
count_speedup_and_efficiency(timings)
for feature in ("speedup", "efficiency"):
    create_plot(timings, feature)
