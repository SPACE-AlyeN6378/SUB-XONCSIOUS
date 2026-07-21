from matplotlib import pyplot as plt
import os
import sys
import csv
import os


def main(path="samples.csv"):
	# allow passing path as CLI arg
	if len(sys.argv) > 1:
		path = sys.argv[1]
	if not os.path.isabs(path):
		path = os.path.join(os.path.dirname(__file__), path)

	xs = []
	ys = []
	with open(path, newline='') as f:
		reader = csv.reader(f)
		header = next(reader, None)
		for row in reader:
			if not row:
				continue
			try:
				# try first two columns
				x = float(row[0])
				y = float(row[1])
			except Exception:
				continue
			xs.append(x)
			ys.append(y)

	if not xs:
		print(f"No numeric data found in {path}")
		return

	plt.figure()
	plt.plot(xs, ys, marker='o')
	plt.xlabel(header[0] if header else 'x')
	plt.ylabel(header[1] if header and len(header) > 1 else 'y')
	plt.title(f"Plot of {os.path.basename(path)}")
	plt.grid(True)
	plt.tight_layout()
	plt.show()


if __name__ == '__main__':
	main()
	os.remove("tests/samples.csv")



