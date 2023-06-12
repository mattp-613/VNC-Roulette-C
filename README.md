# VNC-Roulette
Notice:

Use the scripts included in this repository responsibly. Everything here is made for educational and/or research purposes.

Intro:

This is a script that will grab screenshots of IP's VNC from IP's given in a .txt file

Requirements:

GCC and G++ to compile & the vncsnapshot package. All libraries required to compile are on the top of the file (glibc and stuff)
The compiled binary is included in the .out file.
To compile, you may need to add the -pthread flag or the -lrt flag. I don't know what the -lrt flag does but it sometimes works.

A Linux machine is heavily recommended as the commands executed by this script were tested and ran on a Linux machine. These commands can be seen in the script if interested.

Instructions:

Make an iplist.txt file and run the script. You can use the shellscript provided to compile and run or run the binary itself.
Python scripts are provided to both use a mass scanner VNCMassscan.py on a specified range of IP's and to parse them into a long list of just the IP's using IPparser.py

Difference Between Python Script:

This script is faster and has less issues, and has way less issues setting up the environment to run the script.