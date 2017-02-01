#!/usr/bin/python3

import sys
import subprocess

matchCount = int(sys.argv[1])

count1 = 0;
count2 = 0;

# improve time with threading https://pymotw.com/3/threading/
for i in range(matchCount):
	p = subprocess.Popen("./simpleNvN.sh", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	out, err = p.communicate()
	lines = out.splitlines()
	
	line = lines[-1].decode("utf-8")
	if 'Player #2' in line: # sanity check
		if 'rank #1' in line:
			count2 += 1
			print(str(i+1) + ": Player 2 won.")
		else:
			count1 += 1
			print(str(i+1) + ": Player 1 won.")

print("Player 1 won " + str(count1) + " times.")
print("Player 2 won " + str(count2) + " times.")
2