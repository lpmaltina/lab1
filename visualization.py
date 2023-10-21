#!/usr/bin/env python3

from matplotlib import pyplot as plt
from celluloid import Camera
from tqdm import tqdm


def read_input_data(filename):
    data = [{"x": [], "y": []}]
    with open(filename, "r", encoding="utf-8") as f:
        _, bodies, _ = f.readline().split()
        bodies = int(bodies)
        for _ in range(bodies):
            f.readline()  # mass
            x, y = f.readline().split()
            data[0]["x"].append(float(x))
            data[0]["y"].append(float(y))
            f.readline()  # velocity
    return data


def read_output_data(filename, data, bodies, time_steps):
    with open(filename, "r", encoding="utf-8") as f:
        f.readline()  # header
        for i in range(time_steps):
            lineCur = f.readline()
            if not lineCur:
                break
            _, *coords = lineCur.split()
            data.append({"x": [], "y": []})
            for j in range(bodies):
                x = float(coords[j * 2])
                y = float(coords[j * 2 + 1])
                data[-1]["x"].append(x)
                data[-1]["y"].append(y)


def create_animation(data, bodies, time_steps):
    fig = plt.figure()
    camera = Camera(fig)
    xmin, xmax, ymin, ymax = plt.axis()
    delta_x = (xmax - xmin) / 10
    delta_y = (ymax - ymin) / 10

    for i in tqdm(range(time_steps + 1)):
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


if __name__ == "__main__":
    mode = "serial"
    bodies = 10
    time_steps = 10

    data = read_input_data(f"input/input-10-10")
    read_output_data(f"output/output-{mode}-{bodies}-10", data, bodies, time_steps)
    create_animation(data, bodies, time_steps)

    # for bodies in (10, 100):
    #     for time_steps in (10, 100, 1000):
            
    #         read_output_data(f"output/output-{mode}-{bodies}-{time_steps}", data, bodies, time_steps)
    #         create_animation(data, bodies, time_steps)
