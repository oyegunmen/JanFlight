"""
IMU Roll / Pitch / Yaw 3D Orientation Visualizer
--------------------------------------------------
Reads lines like:  Roll:12.34,Pitch:-5.67,Yaw:88.10
from a serial port and rotates a simple 3D wireframe
"drone" model live.

Uncomment the printRollPitchYaw() function in main loop and flash the code

Create virtual environment inside JanFlight Directory
    python -m venv venv

Install Requirements:
    pip install pyserial matplotlib numpy

Activate venv
venv\Scripts\activate

Run:
    python imu_visualizer.py --port COM4 --baud 500000
"""

import argparse
import re
import threading
import time

import numpy as np
import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Line3DCollection

# ------------------------------------------------------------------
# 1. ARGUMENTS
# ------------------------------------------------------------------
parser = argparse.ArgumentParser()
parser.add_argument("--port", required=True, help="Serial port, e.g. COM4 or /dev/ttyACM0")
parser.add_argument("--baud", type=int, default=500000, help="Baud rate (match Serial.begin in Arduino)")
parser.add_argument("--fps", type=int, default=40, help="Visual refresh rate (independent of IMU data rate)")
args = parser.parse_args()

YAW_OFFSET_DEG = 135

# ------------------------------------------------------------------
# 2. SHARED STATE (updated by serial thread, read by plot thread)
# ------------------------------------------------------------------
latest_angles = {"roll": 0.0, "pitch": 0.0, "yaw": 0.0}
lock = threading.Lock()
stop_flag = False

LINE_RE = re.compile(
    r"Roll:(-?\d+\.?\d*),Pitch:(-?\d+\.?\d*),Yaw:(-?\d+\.?\d*)"
)

def serial_reader():
    """Runs in background thread. Reads serial as fast as data arrives."""
    global stop_flag
    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except serial.SerialException as e:
        print(f"[ERROR] Could not open serial port {args.port}: {e}")
        stop_flag = True
        return

    print(f"[INFO] Listening on {args.port} @ {args.baud} baud...")
    while not stop_flag:
        try:
            raw = ser.readline().decode("utf-8", errors="ignore").strip()
        except Exception:
            continue
        if not raw:
            continue
        match = LINE_RE.search(raw)
        if match:
            r, p, y = map(float, match.groups())
            with lock:
                latest_angles["roll"] = r
                latest_angles["pitch"] = p
                latest_angles["yaw"] = y
    ser.close()


# ------------------------------------------------------------------
# 3. DRONE MODEL (simple placeholder wireframe)
#    Replace `get_model_points()` with your own vertex/line set to
#    swap in a plane, wing, or any other shape.
# ------------------------------------------------------------------
def get_model_lines():
    """
    Returns a list of line segments (each a pair of 3D points) that
    make up the placeholder quadcopter shape, defined in the body
    frame:  X = nose/forward, Y = right, Z = up.
    """

    arm = 2.0

    # Center body cross (X arm, Y arm)
    lines = [
        [(0, 0, 0), (arm, arm, 0)],          # front right
        [(0, 0, 0), (-arm, -arm, 0)],         # back left
        [(0, 0, 0), (arm, -arm, 0)],          # back right
        [(0, 0, 0), (-arm, arm, 0)],         # front left
    ]

    # Small motor markers (circles approximated as squares) at arm tips
    tip_size = 0.3
    tips = [(arm, arm, 0), (-arm, -arm, 0), (arm, -arm, 0), (-arm, arm, 0)]
    for tx, ty, tz in tips:
        lines.append([(tx - tip_size, ty, tz), (tx + tip_size, ty, tz)])
        lines.append([(tx, ty - tip_size, tz), (tx, ty + tip_size, tz)])

    return lines


def get_nose_line():
    """A distinct forward-pointing line so orientation is unambiguous."""
    return [(0, 0, 0), (1.5, 0, 0)]


# ------------------------------------------------------------------
# 4. ROTATION MATH
# ------------------------------------------------------------------
def rotation_matrix(roll_deg, pitch_deg, yaw_deg):
    """Standard ZYX rotation (yaw, pitch, roll)."""
    r = np.radians(roll_deg)
    p = np.radians(pitch_deg)
    y = np.radians(yaw_deg)

    Rz = np.array([[np.cos(y), -np.sin(y), 0],
                   [np.sin(y),  np.cos(y), 0],
                   [0,          0,         1]])

    Ry = np.array([[np.cos(p), 0, np.sin(p)],
                   [0,         1, 0],
                   [-np.sin(p),0, np.cos(p)]])

    Rx = np.array([[1, 0,          0],
                   [0, np.cos(r), -np.sin(r)],
                   [0, np.sin(r),  np.cos(r)]])

    return Rz @ Ry @ Rx


def apply_rotation(lines, R):
    rotated = []
    for p1, p2 in lines:
        p1r = R @ np.array(p1)
        p2r = R @ np.array(p2)
        rotated.append((p1r, p2r))
    return rotated


# ------------------------------------------------------------------
# 5. PLOT SETUP
# ------------------------------------------------------------------
fig = plt.figure(figsize=(7, 7))
ax = fig.add_subplot(111, projection="3d")
fig.patch.set_facecolor("#101418")
ax.set_facecolor("#101418")

AXIS_LIM = 1.6
ax.set_xlim(-AXIS_LIM, AXIS_LIM)
ax.set_ylim(-AXIS_LIM, AXIS_LIM)
ax.set_zlim(-AXIS_LIM, AXIS_LIM)
ax.set_box_aspect([1, 1, 1])
ax.set_axis_off()
ax.view_init(elev=25, azim=-60)

body_lines = get_model_lines()
nose_line = get_nose_line()

body_collection = Line3DCollection(body_lines, colors="#00d2ff", linewidths=2.5)
nose_collection = Line3DCollection([nose_line], colors="#ff3b3b", linewidths=3.5)
ax.add_collection3d(body_collection)
ax.add_collection3d(nose_collection)

# Static reference world axes (faint) so you can see absolute orientation
axis_len = 2.0
ax.plot([0, axis_len], [0, 0], [0, 0], color="gray", alpha=0.3, linewidth=1)
ax.plot([0, 0], [0, axis_len], [0, 0], color="gray", alpha=0.3, linewidth=1)
ax.plot([0, 0], [0, 0], [0, axis_len], color="gray", alpha=0.3, linewidth=1)

angle_text = fig.text(
    0.98, 0.03, "", ha="right", va="bottom",
    fontsize=13, color="white", family="monospace",
    bbox=dict(facecolor="#1c1f24", edgecolor="#00d2ff", boxstyle="round,pad=0.5")
)

title_text = fig.text(0.02, 0.96, "IMU Orientation Tracker", fontsize=14,
                       color="#00d2ff", family="monospace")


def update_plot():
    with lock:
        roll = latest_angles["roll"]
        pitch = latest_angles["pitch"]
        yaw = latest_angles["yaw"]

    R_live = rotation_matrix(roll, pitch, yaw)
    R_offset = rotation_matrix(0, 0, YAW_OFFSET_DEG)   # pure Z-axis offset
    R = R_offset @ R_live

    rotated_body = apply_rotation(body_lines, R)
    rotated_nose = apply_rotation([nose_line], R)

    body_collection.set_segments(rotated_body)
    nose_collection.set_segments(rotated_nose)

    angle_text.set_text(
        f"Roll : {roll:6.1f} deg\nPitch: {pitch:6.1f} deg\nYaw  : {yaw:6.1f} deg"
    )

    fig.canvas.draw_idle()


# ------------------------------------------------------------------
# 6. MAIN LOOP
# ------------------------------------------------------------------
def main():
    reader_thread = threading.Thread(target=serial_reader, daemon=True)
    reader_thread.start()

    plt.ion()
    plt.show()

    frame_time = 1.0 / args.fps
    try:
        while not stop_flag:
            start = time.time()
            update_plot()
            plt.pause(0.001)
            elapsed = time.time() - start
            time.sleep(max(0.0, frame_time - elapsed))
    except KeyboardInterrupt:
        pass
    finally:
        global stop_flag_outer
        globals()["stop_flag"] = True


if __name__ == "__main__":
    main()