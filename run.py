import os
from struct import calcsize
import subprocess
import random
import sys
import getopt
import time
from time import sleep
from collections import namedtuple


configTuple = namedtuple('run_tuple', ['n', 'k', 'alpha', 'beta', 'reds', 'blues', 'byzantines'])


def run_nodes(run_tuple: configTuple, results_file):
    write_config_object(run_tuple)

    colors = ['R', 'B', 'U']

    times_file = open("files/times.txt", "w")
    times_file.close()

    timeout = run_tuple.n

    for i in range(1, run_tuple.n + 1):
        file_name = "files/config_" + str(i) + ".txt"

        """
        num = random.randint(0, 2)
        byzantine = random.randint(0, 1)
        """

        config_file = open(file_name, "w")
        config_file.write(str(i) + "\n")

        if i <= run_tuple.reds:
            config_file.write(colors[0] + "\n")
        elif i <= run_tuple.reds + run_tuple.blues:
            config_file.write(colors[1] + "\n")
        else:
            config_file.write(colors[2] + "\n")

        if i <= run_tuple.byzantines:
            config_file.write("1\n")
        else:
            config_file.write("0\n")

        config_file.close()

    processes = []
    p = subprocess.Popen(["cmake/build/coordinator_main", f"{run_tuple.n}"])
    processes.append(p)


    for i in range(1, run_tuple.n + 1):
        p = subprocess.Popen(
            ["cmake/build/node_main", f"{i}", f"{run_tuple.n}", f"{run_tuple.k}", f"{run_tuple.alpha}", f"{run_tuple.beta}"])
        processes.append(p)
        # p.communicate()

    start = time.time()
    while True:
        with open(r"files/times.txt", 'r') as times_file:
            lines = times_file.readlines()
            x = len(lines)
            current_time = time.time()
            # if current_time - start > timeout:
            #     print(f"Timed out!\nCould not reach a decision within {timeout} seconds")
            #     break
            if x >= run_tuple.n:
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
                        blue_nodes += 1
                    blues_percentage = float(blue_nodes) / float(run_tuple.n)
                    reds_percentage = float(red_nodes) / float(run_tuple.n)

                results_file.write("The decision was: " + ("Blue" if blues_percentage > reds_percentage else "Red") + "\n")
                results_file.write("The maximal time it took is:" + str(max_time) + "\n")
                results_file.write(str(max(blues_percentage, reds_percentage)*100) + "% of the nodes reached the same the decision\n")

                print("The decision was: ", "Blue" if blues_percentage > reds_percentage else "Red")
                print("The maximal time it took is:" + str(max_time))
                print(max(blues_percentage, reds_percentage)*100, "% of the nodes reached the same the decision")
                break
        sleep(1)

    for p in processes:
        p.kill()


def write_config_object(run_tuple: configTuple):
    general_config_file = open("files/general_config.txt", "w")
    general_config_file.write(str(run_tuple.n) + "\n")
    general_config_file.write(str(run_tuple.k) + "\n")
    general_config_file.write(str(run_tuple.alpha) + "\n")
    general_config_file.write(str(run_tuple.beta) + "\n")
    general_config_file.close()


def main():

    file_name = "files/results.txt"

    results_file = open(file_name, "w")
    results_file.write("The Results of 5 trials: \n")
    # tuples = generate_concecutive_tuples()
    # tuples = generate_basic_tuples()
    # tuples = generate_minimum_tuples()
    tuples = [(configTuple(10, 4, 3, 2, 6, 3, 1))]

    for t in tuples:
        results_file.write("the configiration is : " + str(t) + "\n")
        run_nodes(run_tuple=t,results_file=results_file)



def generate_minimum_tuples():
    tuples = [(configTuple(100, 40, 25, 5, 60, 35, 10)),
              (configTuple(100, 40, 25, 5, 60, 35, 20)),
              (configTuple(100, 40, 25, 5, 60, 40, 10)),
              (configTuple(100, 30, 20, 5, 60, 35, 10)),
              (configTuple(100, 30, 20, 5, 25, 60, 10))]
    return tuples


def generate_basic_tuples():
    tuples = []
    for n in range(100, 500, 100):
        for k in range(int(n/5), int(4*n/5), int(n/5)):
            alphas = [int(k/2+1)]  # , int(2*k/3)]
            for alpha in alphas:
                betas = [2, 4]
                for beta in betas:
                    reds = [int(4*n/10)]  # , int(7*n/10)]
                    for red in reds:
                        uncoloreds = [int(0.15*n)]  # , int(0.1*n)]
                        for uncolored in uncoloreds:
                            blue = n-red-uncolored
                            for byzantine in range(int(0.05 * n), int(0.35 * n), int(0.1 * n)):
                                tuples.append(configTuple(n, k, alpha, beta, red, blue, byzantine))
    return tuples


def generate_concecutive_tuples():
    tuples = []

    for n in range(200, 1000, 50):
        for k in range(int(n/5), int(4*n/5), int(n/5)):
            for alpha in range(int(k/2)+1, int(3*k/4), int(k/8)):
                for beta in range(2, 10, 2):
                    for reds in range(int(n/5), int(9*n/10), int(n/5)):
                        for uncolored in range(int(n/20), int(n/10), int(n/20)):
                            blues = n-reds-uncolored
                            for byzantines in range(int(0.05*n), int(0.35*n), int(0.05*n)):
                                tuples.append(configTuple(n, k, alpha, beta, reds, blues, byzantines))

    return tuples


if __name__ == "__main__":
    main()
