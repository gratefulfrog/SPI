#!/usr/bin/python3

import sys

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: $ ./master.py s_init_t, s_payload_t'  )
        print('s_payload_t will be repeated to continue comms indefinitely...')
        print('Note: the AEM board must be running the appropriate software, corresponding to the <type>')
        sys.exit(0)

    comms_x = 'x'
    comms_y = 'y'
    comms_s_init_t = 'ssss'
    comms_s_payload_t = 'pppppp'
    print(sys.argv)
    channelVec = [0] if len(sys.argv)==3 else list(map(int,sys.argv[1:-2]))
    tyLis = [channelVec] + list(map(lambda s:eval('comms_'+s),sys.argv[-2:]))

    print(tyLis)
            
                 
