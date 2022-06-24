import os
from struct import calcsize
import subprocess
import random
import sys
import getopt
import time
from time import sleep


def run_nodes(n, k, alpha, beta, reds, blues, byzantines):
    num_of_nodes = int(n)
    processes = []

    general_config_file = open("files/general_config.txt", "w")
    general_config_file.write(str(num_of_nodes) + "\n")
    general_config_file.write(str(k) + "\n")
    general_config_file.write(str(alpha) + "\n")
    general_config_file.write(str(beta) + "\n")
    general_config_file.close()

    times_file = open("files/times.txt", "w")
    times_file.close()

    for i in range(1, num_of_nodes + 1):
        file_name = "files/config_" + str(i) + ".txt"
        
        
        colors = ['R', 'B', 'U']
        """
        num = random.randint(0, 2)
        byzantine = random.randint(0, 1)
        """

        config_file = open(file_name, "w")
        config_file.write(str(i) + "\n")
        if i <= reds:
            config_file.write(colors[0] + "\n")
        elif i <= reds + blues:
            config_file.write(colors[1] + "\n")
        else:
            config_file.write(colors[2] + "\n")
        if i <= byzantines:
            config_file.write("1\n")
        else:
            config_file.write("0\n")
        config_file.close()

    for i in range(1, num_of_nodes + 1):
        p = subprocess.Popen(
            ["cmake/build/node", f"{i}", f"{n}", f"{k}", f"{alpha}", f"{beta}"])
        processes.append(p)
        # p.communicate()

    
    while True:
        with open(r"files/times.txt", 'r') as times_file:
            lines = times_file.readlines()
            x = len(lines)
            if x >= int(n):
                print("All nodes finished!")
                max_time = 0
                red_nodes = 0
                blue_nodes = 0
                for line in lines:
                    splitted = line.split()
                    finish_time = int(splitted[1])
                    max_time = max(max_time, finish_time)
                    if splitted[0] == "R":
                        red_nodes += 1
                    elif splitted[0] == "B":
                        blue_nodes +=1
                    blues_percentage = float(blue_nodes) / float(n)
                    reds_percentage = float(red_nodes) / float(n)
                print("The maximal time it took is:" + str(max_time))
                print(max(blues_percentage, reds_percentage)*100,"% of the nodes reached the same the decision")
                break
        sleep(1)
             
    for p in processes:
        p.wait()


def main():
    n = ''
    k = ''
    alpha = ''
    beta = ''

    """
    try:
        opts, args = getopt.getopt(argv, "n:k:a:b:", ["alpha=", "beta="])
    except getopt.GetoptError:
        print("Wrong format")
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-n':
            n = arg
        elif opt == '-k':
            k = arg
        elif opt in ('-a', '--alpha'):
            alpha = arg
        elif opt in ('-b', '--beta'):
            beta = arg
    """
    tuples = [(100, 40, 25, 2, 60, 35, 5, 5)]
    for t in tuples:
        run_nodes(n=t[0], k=t[1], alpha=t[2], beta=t[3], reds=t[4], blues=t[5], byzantines=t[6])


if __name__ == "__main__":
    main()
