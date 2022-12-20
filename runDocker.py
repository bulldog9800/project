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


def run_nodes(run_tuple: configTuple):
    write_config_object(run_tuple)

    colors = ['R', 'B', 'U']
    processes = []

    for i in range(1, run_tuple.n + 1):
        color = 'U'
        byzantine = '1' if i <= run_tuple.byzantines else '0'

        if i <= run_tuple.reds:
            color = 'R'
        elif i <= run_tuple.reds + run_tuple.blues:
            color = 'B'

        p = subprocess.run(["docker", "run", "-d", "--name", f"node{i}", "--entrypoint", "node_main.cc" , "ubuntu:verion2",
                         f"{i}", f"{run_tuple.n}", f"{run_tuple.k}", f"{run_tuple.alpha}", f"{run_tuple.beta}",
                          color, byzantine])
        processes.append(p)

    while True:
        command1 = "get --prefix finish/"
        finish_results=exc_etcd_command(command1)
        results_array = finish_results.split('\n')
        if len(results_array) == 4*run_tuple.n:
            print("All nodes finished!")
        command2 = "get --prefix finish/color"
        finish_results = exc_etcd_command(command2)
        color_results_array = finish_results.split('\n')
        red_nodes = 0
        blue_nodes = 0
        for c in color_results_array:
            if c == 'R':
                red_nodes += 1
            elif c == 'B':
                blue_nodes +=1
        blues_percentage = float(blue_nodes) / float(run_tuple.n)
        reds_percentage = float(red_nodes) / float(run_tuple.n)
        command3 = "get --prefix finish/time"
        finish_results = exc_etcd_command(command3)
        time_results_array = finish_results.split('\n')
        max_time=0
        for t in time_results_array:
            max_time = max(max_time, int(t))
        print("The decision was: ", "Blue" if blues_percentage > reds_percentage else "Red")
        print("The maximal time it took is:" + str(max_time))
        print(max(blues_percentage, reds_percentage) * 100, "% of the nodes reached the same the decision")
        break

    for p in processes:
        p.kill()


def write_config_object(run_tuple: configTuple):
    general_config_file = open("files/general_config.txt", "w")
    general_config_file.write(str(run_tuple.n) + "\n")
    general_config_file.write(str(run_tuple.k) + "\n")
    general_config_file.write(str(run_tuple.alpha) + "\n")
    general_config_file.write(str(run_tuple.beta) + "\n")
    general_config_file.close()


def compile_cmake_project():
    subprocess.run(["cmake", "-DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..", "cmake/build"])
    subprocess.run(["make", "-j4"])

def exc_etcd_command(command):
    result = subprocess.run(["etcdctl", "--endpoints 172.20.0.5:2379", command], stdout=subprocess.PIPE)
    if result.returncode == 0:
        return result.stdout.decode
    else:
        print("Error executing etcdctl command")
        return None



def main():
    compile_cmake_project()
    # tuples = generate_concecutive_tuples()
    # tuples = generate_basic_tuples()
    # tuples = generate_minimum_tuples()
    tuples = [(configTuple(10, 4, 3, 2, 6, 3, 1))]

    for t in tuples:
        run_nodes(run_tuple=t)



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
