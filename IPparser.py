#To be used after running VNCmassscan.py
#To parse the list of IP's to remove all
#The bullshit from the masscan script

import os
import shutil

def parseIPs(toFormat, ipFile):
    if os.path.isfile(ipFile):
        print('List of ips detected.')
        with open(ipFile) as f:
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
            shutil.copyfile(toFormat,ipFile)
            f.close()

def main():
    toFormat = "ips.txt" #file name of ips that have to be formatted
    ipFile = "iplist.txt" #replace name of file here
    parseIPs(toFormat, ipFile)

if __name__ == '__main__':
    main()