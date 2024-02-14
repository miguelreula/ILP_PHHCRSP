# PHHCRSP

Source code

This is the windows version of the program. Has been compiled with visual studio 2017. Source files are in standard c++
Running the solver

ILP_PHHCRSP	Instances <input_file> <solution_file> <timeout> <random_seed>

For example, the command line:

./mrils ../Instances/Public/wlp18.dzn sol18.txt 200 0

Runs for 200s the solver on instance wlp18.dzn stored in the directory ../Instances/Public/ and delivers the solution in the file sol18.txt

ILP_PHHCRSP	Instances 0407-8-50-5-1.txt -m 3 -t 600 -o 4 -c 8 -t 60 
