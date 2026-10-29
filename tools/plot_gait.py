#!/usr/bin/env python3
"""Plot the gait CSV from build/gait_dump.

Makes two images:
  gait_joints.png - joint angles over time
  gait_legs.png   - stick drawing of both legs at a few points in the cycle
"""
import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from leg_ik import leg_points  # noqa: E402


def load(path):
    with open(path) as f:
        rows = list(csv.DictReader(f))
    return [{k: float(v) for k, v in r.items()} for r in rows]


def plot_joints(rows, out):
    t = [r["t_ms"] / 1000.0 for r in rows]
    fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
    for name in ("L_HIP", "L_KNEE", "L_ANKLE"):
        ax[0].plot(t, [r[name] for r in rows], label=name)
    for name in ("R_HIP", "R_KNEE", "R_ANKLE"):
        ax[1].plot(t, [r[name] for r in rows], label=name)
    ax[2].plot(t, [r["roll"] for r in rows], label="roll")
    ax[2].plot(t, [STAND - r["lz"] for r in rows], label="left lift (mm)")
    ax[2].plot(t, [STAND - r["rz"] for r in rows], label="right lift (mm)")
    for a in ax:
        a.legend(loc="upper right", fontsize=8)
        a.grid(True, alpha=0.3)
    ax[0].set_ylabel("deg")
    ax[1].set_ylabel("deg")
    ax[2].set_xlabel("time (s)")
    fig.tight_layout()
    fig.savefig(out, dpi=100)


def plot_legs(rows, out, frames=8):
    # first walk loop runs from 0.95 s to 3.65 s (see gait.c timings)
    start = next(i for i, r in enumerate(rows) if r["t_ms"] >= 950)
    end = next((i for i, r in enumerate(rows) if r["t_ms"] >= 3650),
               len(rows) - 1)
    idx = [start + (end - start) * k // (frames - 1) for k in range(frames)]

    fig, ax = plt.subplots(1, frames, figsize=(2 * frames, 3), sharey=True)
    for a, i in zip(ax, idx):
        r = rows[i]
        for side, color in (("L", "tab:blue"), ("R", "tab:red")):
            pts = leg_points(r[side + "_HIP"], r[side + "_KNEE"])
            a.plot([p[0] for p in pts], [-p[1] for p in pts], "-o",
                   color=color, label=side)
        a.axhline(-STAND, color="gray", lw=0.5)
        a.set_title("t=%.2fs" % (r["t_ms"] / 1000.0), fontsize=9)
        a.set_xlim(-60, 60)
        a.set_ylim(-110, 10)
        a.set_aspect("equal")
    ax[0].legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(out, dpi=100)


STAND = 95.0


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "build/gait.csv"
    rows = load(path)
    outdir = os.path.dirname(path) or "."
    plot_joints(rows, os.path.join(outdir, "gait_joints.png"))
    plot_legs(rows, os.path.join(outdir, "gait_legs.png"))
    print("plotted %d samples -> %s/gait_joints.png, gait_legs.png"
          % (len(rows), outdir))


if __name__ == "__main__":
    main()
