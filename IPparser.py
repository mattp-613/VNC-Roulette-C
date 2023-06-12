#To be used after running VNCmassscan.py
#To parse the list of IP's to remove all
#The bullshit from the masscan script

import os
import shutil

def parseIPs(toFormat, ipFile):
    ips = []
    if os.path.isfile(toFormat):
        print('File found: {}.'.format(toFormat))
        with open(toFormat) as f:
            lines = f.readlines()
            for line in lines:
                parsedLine = line.split(" ")
                try:
                    ips.append(parsedLine[3])
                except:
                    print("The following line does not work. Skipping line:")
                    print(line)
                    print("")
            f.close()

        #put ips list into a file
        if os.path.isfile(ipFile):
            print('File found: {}.'.format(ipFile))
            with open(ipFile, "a") as f: 
                    f.write('\n'.join(ips))
                    f.write('\n')
            f.close()

def main():
    toFormat = "ips.txt" #file name of ips that have to be formatted
    ipFile = "iplist.txt" #replace name of file here
    parseIPs(toFormat, ipFile)

if __name__ == '__main__':
    main()