# VNC-Roulette
Notice:

Use the scripts included in this repository responsibly. Everything here is made for educational and/or research purposes.

Intro:

This is a script that will grab screenshots of IP's VNC from IP's given in a .txt file

Requirements:

GCC to compile & the vncsnapshot package. All libraries required to compile are on the top of the file (glibc and stuff)
The compiled binary is included in the .out file.

To compile, you may need to add the -pthread flag or the -lrt flag. I don't know what the -lrt flag does but it sometimes works.

Instructions:

Make an iplist.txt file and run the script. You can use the shellscript provided to compile and run or run the binary itself.