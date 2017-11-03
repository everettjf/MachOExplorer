import sys
import os
from datetime import datetime


def parse_dir(dir):
    idx = 0
    for path,dirs,files in os.walk(dir):
        for f in files:
            filepath = os.path.join(path,f)
            print(idx)
            print('echo "./moex-cli --file \"%s\" --header_list"' % filepath)
            print('./moex-cli --file "%s" --header_list' % filepath)
            print('')
            idx+=1

            if idx > 10000:
                return

if __name__ == '__main__':
    dir = '/Applications'
    parse_dir(dir)
