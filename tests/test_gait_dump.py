import csv
import io
import os
import subprocess

import pytest

import leg_ik

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUMP = os.path.join(ROOT, "build", "gait_dump")

pytestmark = pytest.mark.skipif(not os.path.exists(DUMP),
                                reason="run make first")


@pytest.fixture(scope="module")
def rows():
    out = subprocess.run([DUMP, "6000", "20"], capture_output=True,
                         text=True, check=True).stdout
    return [{k: float(v) for k, v in r.items()}
            for r in csv.DictReader(io.StringIO(out))]


def test_row_count_and_columns(rows):
    assert len(rows) == 301
    assert set(["L_HIP", "R_ROLL", "lx", "rz"]) <= set(rows[0])


def test_c_ik_matches_python(rows):
    worst = 0.0
    for r in rows:
        for side, x, z in (("L", r["lx"], r["lz"]), ("R", r["rx"], r["rz"])):
            hip, knee, ankle = leg_ik.ik_leg(x, z)
            worst = max(worst, abs(hip - r[side + "_HIP"]),
                        abs(knee - r[side + "_KNEE"]),
                        abs(ankle - r[side + "_ANKLE"]))
    # CSV is printed with 2 decimals, inputs are rounded too
    assert worst < 0.1


def test_feet_stay_flat(rows):
    # ankle = knee - hip keeps the sole level
    for r in rows:
        for s in "LR":
            assert abs(r[s + "_ANKLE"] - (r[s + "_KNEE"] - r[s + "_HIP"])) < 0.02


def test_python_ik_out_of_reach():
    assert leg_ik.ik_leg(0, 115) is None
    assert leg_ik.ik_leg(0, -10) is None


def test_plot_script(tmp_path):
    csv_path = tmp_path / "gait.csv"
    with open(csv_path, "w") as f:
        subprocess.run([DUMP, "6000", "20"], stdout=f, check=True)
    subprocess.run(["python3", os.path.join(ROOT, "tools", "plot_gait.py"),
                    str(csv_path)], check=True, capture_output=True)
    assert (tmp_path / "gait_joints.png").stat().st_size > 10000
    assert (tmp_path / "gait_legs.png").stat().st_size > 10000
