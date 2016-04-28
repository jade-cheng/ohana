#!/usr/bin/python

import matplotlib
import numpy
import pylab
import sys

assert len(sys.argv) == 3, 'usage: plot-q.py <q-file> <image_out>'

q_file = sys.argv[1]
image_out = sys.argv[2]

with open(q_file, 'r') as f:
    lines = f.readlines()

q_data = []

rows, cols = map(int, lines[0].split())

for line in lines[1:]:
    entries = map(float, line.split())
    assert len(entries) == cols
    q_data.append(entries)

data = numpy.transpose(numpy.array(q_data))
figure = pylab.figure(figsize=(120, 4))
axis = figure.add_subplot(1, 1, 1)
ind = numpy.arange(data.shape[1])
bottom = numpy.vstack((numpy.zeros((data.shape[1],), dtype=data.dtype),
                      numpy.cumsum(data, axis=0)[:-1]))

for dat, col, bot in zip(data, matplotlib.colors.cnames.iteritems(), bottom):
    axis.bar(ind, dat, color=col, bottom=bot, alpha=1.0, width=1.0)

axis.get_xaxis().set_visible(False)
axis.get_yaxis().set_visible(False)
axis.set_ylim(0.0, 1.0)
axis.set_xlim(0.0, data.shape[1])
pylab.savefig(image_out, bbox_inches='tight')
