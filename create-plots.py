import matplotlib.pyplot as plt

n_bodies = (64, 128, 256, 512, 1024)
n_time_steps = (10, 100, 1000)


def read_timings(filename):
    timings = {}
    with open(filename, "r", encoding="utf-8") as f:
        line = f.readline().strip()
        if line:
            threads, bodies, time_steps, time = line.split("\t")
            key = f"{bodies}-{time_steps}"
            if key in timings:
                timings[key]["time"][threads] = time
            else:
                timings[key]["time"] = {threads: time}
    return timings


def read_timings(filename):
    timings = {}
    with open(filename, "r", encoding="utf-8") as f:
        f.readline()  # header
        for line in f:
            line = line.strip()
            if line:
                threads, bodies, time_steps, time = line.split("\t")
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


def count_speedup_and_efficiency(timings):
    for bodies in n_bodies:
        for time_steps in n_time_steps:
            key = f"{bodies}-{time_steps}"
            one_thread_time = timings[key]["time"][1]
            timings[key]["speedup"] = {}
            timings[key]["efficiency"] = {}
            for threads, info in timings[key]["time"].items():
                timings[key]["speedup"][threads] = one_thread_time / info
                timings[key]["efficiency"][threads] = timings[key]["speedup"][threads] / threads


def create_plot(timings, feature):
    _, axs = plt.subplots(1, len(n_time_steps), figsize=(5 * len(n_time_steps), 5))
    plt.suptitle(feature.title())
    for bodies in n_bodies:
        for i, time_steps in enumerate(n_time_steps):
            key = f"{bodies}-{time_steps}"
            label = f"{bodies} bodies"
            axs[i].set_title(f"{time_steps} time steps")
            axs[i].plot(timings[key][feature].keys(), timings[key][feature].values(), label=label)
    plt.legend()
    plt.savefig(f"plots/{feature}.png")


timings = read_timings("timings")
count_speedup_and_efficiency(timings)
for feature in ("speedup", "efficiency"):
    create_plot(timings, feature)
