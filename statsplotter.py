""" Plot statistics data

usage: python frame_plot.py <stats file>.json

"""

import sys
import json
import matplotlib.pyplot as plt


json_filename = sys.argv[1]

with open(json_filename, "r", encoding="utf-8") as fd:
    stats = json.load(fd)

iframe = []
frame_size = []
proc_time = []

for frame in stats["frames"]:
    iframe.append(frame["iframe"])
    frame_size.append(frame["size"])
    proc_time.append(frame["proctime"])

fig, ax = plt.subplots()
ax2 = ax.twinx()
ax3 = ax.twinx()

plt.title(stats["encodedfile"])
plt.xlim(0, len(iframe))

# Plot I frame interval
ax.plot(iframe, color="red")
ax.set_xlabel("frame count")
ax.set_ylabel("is I-frame", color="blue")
ax.set_ylim(bottom=0, top=max(iframe))

# Plot Frame size in bytes
ax2.plot(frame_size, color="blue")
ax2.set_ylabel("frame size [bytes]", color="red")
ax2.set_ylim(bottom=0, top=max(frame_size))

# Plot proc time in ms
ax3.plot(proc_time, color="green")
ax3.set_ylabel("proctime [ms]", color="green")
ax3.set_ylim(bottom=0, top=max(proc_time))
ax3.spines["right"].set_position(("outward", 60))

plt.show()
