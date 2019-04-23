# program-output-comparator
compiles multiple c files, run them with specified input, compares their output to wanted.  makes .csv file with results of comparison.

ex32.c use ex31.c in order to compare 2 text files.
instructions:
make configuration text file with the following text:
first line: absolute path to the main folder of subfolders with .c files.
second line: absolute path to text file with wanted input check to the programs above.
third line: absolute path to text file with my wanted output to compare with the output of the programs above.

compile ex32.c.
run compiled program with the configuration absolute path as the only parameter.
