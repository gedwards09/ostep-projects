#! /usr/bin/env python3

import random
import string

for i in range(1000000):
	x = ''
	for i in range(1):
		letter = random.choice(string.ascii_lowercase)
		x += letter
	print(x, end=' ')
