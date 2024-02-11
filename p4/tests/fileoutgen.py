#! /usr/bin/python

import sys,os,glob

def generate_kv_pairs(file, dic):
	for line in file:
		for word in line.split():
			if not word in dic:
				dic[word] = 1
			else:
				dic[word] += 1
	return dic

if len(sys.argv) < 2:
	print("exception raised")
	raise Exception()

kv_pairs = {}
parent = sys.argv[1]
d = parent + "/in"
for filename in glob.glob(os.path.join(d, '*.txt')):
	with open(filename, "r") as f:
		kv_pairs = generate_kv_pairs(f, kv_pairs)

filename_out = parent + "/" + parent + "-out-expected.txt"
with open(filename_out, "w") as f:
	for key in sorted(kv_pairs.keys()):
		f.write("{0} {1}\n".format(key, kv_pairs[key]))
