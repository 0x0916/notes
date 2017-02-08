$ gcc -o popen-glob popen-glob.c 
$ ./popen-glob 
pattern: popen-glob*		# matches two filenames
popen-glob
popen-glob.c
	2 matching files
	pclose() status == 0
pattern: x*			# matches no filename
	0 matching file
	pclose() status == 0x200
pattern: ^D$ 			# type Control-D to terminate
