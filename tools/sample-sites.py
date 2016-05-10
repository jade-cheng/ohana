#!/usr/bin/python

import math
import sys

assert len(sys.argv) == 4, \
    'usage: sample-sites.py <gm-file> <percentage> <gm-sampled-file>'


def bresenham(dx, dy):
    d = dy - dx
    for x in range(dx):
        if d >= 0:
            yield x
            d -= dx
        d += dy


def read_lines(f, count):
    while count > 0:
        vals = f.readline().split()
        if len(vals) > 0:
            yield vals
            count -= 1


def read_matrix_size(f):
    for items in read_lines(f, 1):
        assert len(items) == 2, 'invalid matrix file format'
        return map(int, items)


def main():
    in_file = sys.argv[1]
    percent = int(sys.argv[2])
    out_file = sys.argv[3]

    in_ext = in_file[-4:]
    allowed_exts = {'.dgm': 1, '.lgm': 3}
    assert in_ext in allowed_exts, 'matrix extension must be .dgm or .lgm'
    assert 0 < percent < 100, 'percent must range from 1 to 99'
    assert out_file[-4:] == in_ext, 'inconsistent matrix extensions'

    with open(in_file, 'r') as f_in:
        anchor = f_in.tell()
        rows, cols = read_matrix_size(f_in)
        f_in.seek(anchor)

        idx = tuple(bresenham(cols, cols * percent / 100))
        assert len(idx) > 0, 'percent results in zero markers'

        with open(out_file, 'w') as f_out:
            for _ in xrange(allowed_exts[in_ext]):
                r, c = read_matrix_size(f_in)
                assert r == rows and c == cols, 'inconsistent matrix sizes'
                f_out.write('{0} {1}\n'.format(rows, len(idx)))
                for vals in read_lines(f_in, rows):
                    assert len(vals) == cols, 'inconsistent marker counts'
                    f_out.write('\t'.join([vals[i] for i in idx]) + '\n')

main()
