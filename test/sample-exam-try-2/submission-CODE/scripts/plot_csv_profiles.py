from pathlib import Path
import csv
import sys

import matplotlib.pyplot as plt


def read_csv(path):
    with path.open(newline="") as handle:
        rows = list(csv.DictReader(handle))
    return rows


def plot_exercise1(output_dir):
    files = sorted(output_dir.glob("exercise1_line_y_0p5_lambda_*.csv"))
    if not files:
        return

    plt.figure()
    for path in files:
        rows = read_csv(path)
        x = [float(row["x"]) for row in rows]
        u = [float(row["solution"]) for row in rows]
        label = path.stem.replace("exercise1_line_y_0p5_lambda_", "lambda=")
        plt.plot(x, u, label=label)

    plt.xlabel("x on y=0.5")
    plt.ylabel("u")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_dir / "exercise1_line_y_0p5.png", dpi=200)
    plt.close()


def plot_exercise2_line(output_dir):
    path = output_dir / "exercise2_velocity_magnitude_line_y_eq_x_final.csv"
    if not path.exists():
        return

    rows = read_csv(path)
    x = [float(row["x"]) for row in rows]
    mag = [float(row["velocity_magnitude"]) for row in rows]

    plt.figure()
    plt.plot(x, mag)
    plt.xlabel("x on y=x")
    plt.ylabel("|u| at T")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_dir / "exercise2_velocity_magnitude_line_y_eq_x_final.png", dpi=200)
    plt.close()


def plot_exercise2_probe(output_dir):
    path = output_dir / "exercise2_velocity_magnitude_probe_time.csv"
    if not path.exists():
        return

    rows = read_csv(path)
    time = [float(row["time"]) for row in rows]
    mag = [float(row["velocity_magnitude"]) for row in rows]

    plt.figure()
    plt.plot(time, mag)
    plt.xlabel("time")
    plt.ylabel("|u|(1.25,-0.4)")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_dir / "exercise2_velocity_magnitude_probe_time.png", dpi=200)
    plt.close()


def main():
    output_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("output")
    plot_exercise1(output_dir)
    plot_exercise2_line(output_dir)
    plot_exercise2_probe(output_dir)


if __name__ == "__main__":
    main()
