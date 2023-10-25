# Распределённые вычисления. Лабораторная работа №1

## Решение системы обыновенных дифференциальных уравнений
Используется библиотека pthreads.

Материалы: https://disk.yandex.ru/d/upU8bhTGFCkLiQ

### Файловая структура
- **animations** - анимации, иллюстрирующие движение точек (https://disk.yandex.ru/d/upU8bhTGFCkLiQ/animations)
- **input** - файлы с входными данными о координатах точек и их скоростях (https://disk.yandex.ru/d/upU8bhTGFCkLiQ/input)
- **output** - файлы с координатами точек в течение указанного количества временных шагов (https://disk.yandex.ru/d/upU8bhTGFCkLiQ/output)
- **plots** - графики ускорения (speedup) и эффективности
- **create-plots.py** - код для построения графиков ускорения (speedup) и эффективности. Графики строятся на основе данных о времени выполнения вычислений из файла timings.csv и сохраняются в директорию plots
- **file-utils.h**, **file-utils.c** - код для чтения из файлов директории input и записи в файлы директории output
- **generate-input.c** - код для генерации входных данных. Записывает сгенерированные данные в директорию input
- **lab1-parallel.c** -  параллельная программа. Читает входные данные из директории input. Сохраняет координаты точек в течение указанного количества временных шагов в директорию output. Также записывает данные о времени выполнения вычислений в файл timings.csv
- **lab1-serial.c** - последовательная программа. Читает входные данные из директории input. Сохраняет координаты точек в течение указанного количества временных шагов в директорию output
- **timer.h** - макрос для замерения времени в текущий момент
- **timings.csv** - файл с информацией о времени выполнения вычислений
- **vec.h**, **vec.c** - код для работы с двумерными векторами. Используется в последовательной и параллельной программах
- **visualization.py** - код для создания анимаций, иллюстрирующих движение точек. Читает информацию о координатах точек из директорий input и output. Сохраняет анимации в директорию animations

### Компиляция и запуск кода
**1) generate-input.c**:
```console
$ gcc generate-input -o generate-input
$ ./generate-input
```

**2) lab1-serial.c**
```console
$ gcc lab1-serial.c file-utils.c vec.c -o lab1-serial -lm
$ ./lab1-serial
```

**3) lab1-parallel.c**
```console
$ gcc lab1-parallel.c file-utils.c vec.c -o lab1-parallel -lpthread -lm
$ ./lab1-parallel
```

**4) visualization.py**
```console
$ python3 visualization.py
```

**5) create-plots.py**
```console
$ python3 create-plots.py
```

### Информация о системе
    CPU: 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz
    Cores: 4
    Logical processors: 8

### Результаты
![speedup](https://github.com/lpmaltina/lab1/blob/main/plots/speedup.png)


![efficiency](https://github.com/lpmaltina/lab1/blob/main/plots/efficiency.png)

