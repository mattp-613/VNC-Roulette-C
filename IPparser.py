#To be used after running VNCmassscan.py
#To parse the list of IP's to remove all
#The bullshit from the masscan script

import os
import shutil

def parseIPs(textFile):
    if os.path.isfile(textFile):
        print('List of ips detected.')
        with open(textFile) as f:
            lines = f.readlines()
            ips = []
            for line in lines:
                parsedLine = line.split(" ")
                try:
                    ips.append(parsedLine[3])
                except:
                    print("The following line does not work. Skipping line:")
                    print(line)
                    print("")
            shutil.copyfile(textFile,textFile)
            f.close()

def main():
    ipFile = "iplist.txt" #replace name of file here
    parseIPs(ipFile)

if __name__ == '__main__':
    main()