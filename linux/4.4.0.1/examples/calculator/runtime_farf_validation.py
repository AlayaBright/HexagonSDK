import os, signal, time, sys
pid = os.fork()
if pid:                 #parant
    time.sleep(20)     #witing for completing logcat -s adsprpc
    os.kill(pid, signal.SIGSTOP)
    f = os.path.isfile("temp.txt")
    if f == False :
        print("Failed : calculator_walkthrough is failed")
        os._exit(os.EX_OK)
    file = open('temp.txt','a+')
    file1 = open('farf_sum_logs.txt','w')
    failed =1
    for line in file:
        if 'DSP: sum' in line:
            file1.write(line)
            failed =0
            #print(line)
    file.close()
    file1.close()
    if failed ==1 :
        print("Failed : Runtime FARF feature is not working")
    else :
        print("Success : Runtime logcat DSP: sum logs are recorded")
    os.remove('temp.txt')
else :              # child
    n = len(sys.argv)
    if n >= 2:
        device =sys.argv[1]
        #print(device)
        i =0
        os.system("adb logcat -c")
        i=os.system("python calculator_walkthrough.py -T " + device)
        if i == 0 :
            print("calculator_walkthrough Done, running adb logcat -s adsprpc ....")
            os.system("adb logcat -s adsprpc > temp.txt")
        else :
            print("calculater_walkthrough failed ")
    else :
        print("Target Name is not provided..........")