#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os, sys
from getopt import *
from resource import *
import subprocess

FULL_ANS = {
	"OK": "Answer correct",
	"ANS": "Wrong answer",
	"TLE": "Timeout",
	"RTE": "Runtime error",
	"SIG": "Sigkill",
	"SYS": "System error"
}

def run_test(exe, wej):
	wej = os.open(wej, os.O_RDONLY)
	wyj = os.open("/dev/null", os.O_WRONLY)
	used_time = getrusage(RUSAGE_CHILDREN).ru_utime
	try:
		proc = subprocess.Popen(["./"+exe], bufsize=0, stdin=wej, stdout=wyj, stderr=subprocess.STDOUT)
		proc.wait(timeout=2)
	except subprocess.TimeoutExpired:
		proc.kill()
		os.close(wej)
		os.close(wyj)
		return "TLE", 2
	used_time = getrusage(RUSAGE_CHILDREN).ru_utime - used_time
	os.close(wej)
	os.close(wyj)
	if proc.returncode>0:   ans = "RTE"
	elif proc.returncode<0: ans = "SIG"
	else: ans = "OK"
	return ans, used_time

def errc(c):
	return ("\x1b[32m" if c == 0 else "\x1b[31m") + "%3d" % c + "\x1b[0m"

def okc(c):
	return "\x1b[32m" + "%4d" % c + "\x1b[0m"

def check(exe, pref):
	cALL = cOK = cTLE = cRTE = cSIG = cSYS = 0
	wt = 0.0
	mt = 0.0
	tests = sorted(os.listdir('tests/'))
	print("╔═════╦═══════════════════════════════════╦════════╦═══════════════════╗")
	for test in tests:
		if test.startswith(pref) and test[-4:] == '.cnf':
			wej = 'tests/' + test

			b = ("%-33s"%test[:-4])
			print("║ %3d ║"%(cALL,), b, "║", end="")
			sys.stdout.flush()

			ans, time = run_test(exe, wej)

			c = ("\x1b[32m" if ans=="OK" else "\x1b[31m")
			c += ("%-17s"%FULL_ANS[ans]) + "\x1b[0m"
			print("%6.2fs"%(time,), "║", c, "║")

			wt += time
			if time > mt :
				mt = time
			cALL+=1
			if ans=="OK": cOK+=1
			elif ans=="TLE": cTLE+=1
			elif ans=="RTE": cRTE+=1
			elif ans=="SIG": cSIG+=1
			elif ans=="SYS": cSYS+=1
	cALL = "%4d" % cALL
	cOK = okc(cOK)
	cTLE = errc(cTLE)
	cRTE = errc(cRTE)
	cSIG = errc(cSIG)
	cSYS = errc(cSYS)
	print("╠═════╩════════╦═════════════════╦══════╦═╩════╦═══╩═╦═════╦═════╦═════╣")
	print("║ Total time   ║ %14.2fs ║ ALL  ║ OK   ║ TLE ║ RTE ║ SIG ║ SYS ║" % wt)
	s = "║ Max time     ║ %14.2fs ║" + " %s ║"*6
	print(s % (mt, cALL, cOK, cTLE, cRTE, cSIG, cSYS))
	print("╚══════════════╩═════════════════╩══════╩══════╩═════╩═════╩═════╩═════╝")

def is_exe(f):
	return os.path.isfile(f) and os.access(f, os.X_OK)

def is_dir(f):
	return os.path.isdir(f)

def main():
	if len(sys.argv) != 3:
		print("Usage: benchmark.py BIN TEST_PREF")
		sys.exit(0)
	if not is_exe(sys.argv[1]):
		print("Wrong binary file (check isfile & isexecutable)")
		print("Usage: benchmark.py BIN TEST_PREF")
		sys.exit(0)
	check(sys.argv[1], sys.argv[2])

if __name__=="__main__": main()

