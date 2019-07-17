#!/bin/python3
import pathlib
import matplotlib.pyplot as plt
import argparse
import sys

data = {
    "F": ([], "Frame time"),
    "R": ([], "Render time"),
    "U": ([], "Update time"),
    "I": ([], "Input time"),
}

def cliparse():
    parser = argparse.ArgumentParser(description="Game loop performance plotter")
    parser.add_argument('-f', '--file', default='', type=str, required=False, help="load data from a input filename")
    parser.add_argument('-m', '--max-limit', default=None, type=int, required=False, help="limit the input by some number of whole miliseconds (inclusive)")
    return parser.parse_args()

def parse(opened_stream):
    for raw in opened_stream:
        key, delimiter, value = raw.strip().partition(' ')
        if delimiter == ' ' and len(key) == 1 and len(value) > 0:
            yield key.upper(), float(value)

def fillData(opened_stream, upper_limit):
    for key, number in parse(opened_stream):
        if data.get(key) is not None and (upper_limit is None or number <= upper_limit):
            data[key][0].append(number)

def plotit(dataKey):
    pl, = plt.plot(data[dataKey][0], label=data[dataKey][1])
    return pl

def main(args):
    if len(args.file) > 0:
        dataPath = pathlib.Path(args.file)
        print("Parsing data from file {!r}...".format(dataPath.as_posix()))
        fillData(dataPath.open(mode='r'), args.max_limit)
    else:
        try:
            fillData(sys.stdin, args.max_limit)
        except KeyboardInterrupt:
            print("No data received. Exiting.")
            exit(1)

    print("Plotting data...")
    plotlines = tuple( plotit(key) for key in data.keys() )

    print("Rendering graph(s)...")
    plt.grid(True)
    plt.title("Game loop performance")
    plt.ylabel("Time in milliseconds")
    plt.xlabel("Frame sample count")
    plt.legend(plotlines, loc=1)
    plt.tight_layout()
    plt.show()
    print("Done.")

if __name__ == "__main__":
    main(cliparse())