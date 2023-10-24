import matplotlib.pyplot as plt
import sys

x = []
y = []

def process_input(input_source):
    for i in input_source:
        x.append(int(i.strip().split("\t")[0]))
        y.append(float(i.strip().split("\t")[1]))

if len(sys.argv) > 1:  # Check if filename is provided as an argument
    with open(sys.argv[1], 'r') as f:
        process_input(f)
else:
    process_input(sys.stdin)

plt.plot(x, y)
plt.xlabel('Number of reads')
plt.ylabel('Saturation')
plt.title('Saturation Curve')
plt.grid(True)
plt.savefig("Saturation.png", dpi=600)
plt.show()