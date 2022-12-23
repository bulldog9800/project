import os
from struct import calcsize
import subprocess
import random
import sys
import getopt
import time
from time import sleep
from collections import namedtuple
import matplotlib.pyplot as plt



configTuple = namedtuple('run_tuple', ['n', 'k', 'alpha', 'beta', 'reds', 'blues', 'byzantines'])


def run_nodes(run_tuple: configTuple):
    clear_etcd_server()
    delete_Containers()
    #processes = []

    for i in range(1, run_tuple.n + 1):
        color = 'U'
        byzantine = '1' if i <= run_tuple.byzantines else '0'

        if i <= run_tuple.reds:
            color = 'R'
        elif i <= run_tuple.reds + run_tuple.blues:
            color = 'B'

        p = subprocess.run(["docker", "run", "--net", "snowball_cluster", "-d",
         "--name", f"node{i}", "my_ubuntu", 
        "/project/examples/cpp/query/cmake/build/node_main",
        f"{i}", f"{run_tuple.n}", f"{run_tuple.k}", f"{run_tuple.alpha}", f"{run_tuple.beta}", color, byzantine])
        
        #processes.append(p)

    while True:
        finish_results= subprocess.check_output(['docker', 'exec', '-it', 'coordinator', 
                                        '/usr/local/bin/etcdctl', 'get', '--prefix', 'finish/time', 
                                        '--print-value-only'])
        results_array = finish_results.decode().split('\n')
        if len(results_array) == run_tuple.n+1:
            print("All nodes finished!")
            break
        sleep(5)

    max_time=0
    for t in results_array[:-1]:
        t1 = t.rstrip()
        time1 = int(t1)
        max_time = max(max_time, int(time1))

    finish_results2 = subprocess.check_output(['docker', 'exec', '-it', 'coordinator',
                                              '/usr/local/bin/etcdctl', 'get', '--prefix', 'finish/color',
                                              '--print-value-only'])
    color_results_array = finish_results2.decode().split('\n')
    red_nodes = 0
    blue_nodes = 0
    for c in color_results_array:
        color1 = c.rstrip()
        if color1 == 'R':
            red_nodes += 1
        elif color1 == 'B':
            blue_nodes += 1

    blues_percentage = float(blue_nodes) / float(run_tuple.n)
    reds_percentage = float(red_nodes) / float(run_tuple.n)
    color2= "B" if blues_percentage > reds_percentage else "R"
    percentage = max(blues_percentage, reds_percentage) * 100

    print("The decision was: ", color2)
    print("The maximal time it took is:" + str(max_time))
    print(percentage, "% of the nodes reached the same the decision")
    return (max_time,color2,percentage)

def clear_etcd_server():
    subprocess.run(['docker', 'exec', '-it', 'coordinator', '/usr/local/bin/etcdctl', 'del', '--prefix', ""])

def delete_Containers():
    # Get a list of all containers with a name starting with "node"
    containers = subprocess.run(
        ["docker", "container", "ls", "-a", "-q", "--filter", "name=node*"],
        capture_output=True,
        text=True,
    ).stdout.strip().split("\n")

    # Delete all of the containers
    for container in containers:
        subprocess.run(["docker", "container", "rm", "-f", container])

def generate_tuples():
    #n_unstable()
    #k_unstable()
    #alpha_unstable()
    #beta_unstable()
    byzantine_unstable()
    #starting_state_unstable()


def n_unstable():
    results, n_values = [], []
    for n in range(10, 30, 5):
        k, beta, byzantine, red, uncolored = int(0.4*n), 3, int(0.1*n)+1, int(0.4*n), int(0.1*n)
        alpha = int(k/2)+1
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        n_values.append(n)
    
    clear_etcd_server()
    delete_Containers()
    title = 'Time as a function of number of nodes'
    plot_results(unstable_param='n', param_values=n_values, results=results, title=title)


def k_unstable():
    results, k_values = [], []
    for k in [5,7,10,12,15]:
        n, alpha, beta = 25, int(k/2)+1, 3
        byzantine, red, uncolored = int(0.1*n)+1, 0.4*n, int(0.1*n)
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        k_values.append(k)
    clear_etcd_server()
    delete_Containers()
    title = 'Time as a function of sample size'
    plot_results(unstable_param='k', param_values=k_values, results=results, title=title)


def alpha_unstable():
    results, alpha_values = [], []
    for alpha in [6,7,8,10]:
        n, k, beta = 25, 10, 3
        byzantine, red, uncolored = int(0.1*n)+1, 0.4*n, int(0.1*n)
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        alpha_values.append(alpha)
    clear_etcd_server()
    delete_Containers()
    title = 'Time as a function of alpha'
    plot_results(unstable_param='alpha', param_values=alpha_values, results=results, title=title)


def beta_unstable():
    results, beta_values = [], []
    for beta in [2,3,4]:
        n, k, alpha = 25, 10, 7
        byzantine, red, uncolored = int(0.1*n)+1, 0.4*n, int(0.1*n)
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        beta_values.append(beta)

    title = 'Time as a function of beta'
    plot_results(unstable_param='beta', param_values=beta_values, results=results, title=title)


def byzantine_unstable():
    results, byzantine_values = [], []
    for byzantine in [8,9,11,13]:
        n, k, alpha,beta = 25, 10, 7, 3
        red, uncolored = 0.4*n, int(0.1*n)
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        byzantine_values.append(byzantine)
    clear_etcd_server()
    delete_Containers()
    title = 'Time as a function of number of Byzantine nodes'
    plot_results(unstable_param='byzantine', param_values=byzantine_values, results=results, title=title)


def starting_state_unstable():
    results, uncolored_values = [], []
    for uncolored in [0,5,10,15]:
        n, k, alpha,beta = 25, 10, 7, 3
        byzantine, red = int(0.1*n)+1, 0.4*n
        blue = n - red - uncolored
        curr_tuple = configTuple(n, k, alpha, beta, red, blue, byzantine)
        time, color, percentage = run_nodes(run_tuple=curr_tuple)
        results.append((time, color, percentage))
        uncolored_values.append(uncolored)
    clear_etcd_server()
    delete_Containers()
    title = 'Time as a function of number of Uncolored nodes'
    plot_results(unstable_param='uncolored', param_values=uncolored_values, results=results, title=title)


def plot_results(unstable_param, param_values, results, title):
    time_results = [result[0] for result in results]
    plt.scatter(param_values, time_results)
    plt.plot(param_values, time_results)
    plt.xlabel(unstable_param)
    plt.ylabel('time [secs]')
    plt.title(title)
    

    for i, param_value in enumerate(param_values):
        decision = 'RED' if results[i][1] == 'R' else 'BLUE'
        plt.text(param_value, time_results[i], f'(decision={decision}, percentage={results[i][2]})',ha='center',va='center')

    plt.show()



def main():
    #compile_cmake_project()
    # tuples = generate_concecutive_tuples()
    # tuples = generate_basic_tuples()
    # tuples = generate_minimum_tuples()
    #tuples = [(configTuple(30, 10, 6, 2, 20, 5, 1))]
    #tuples = [(configTuple(50, 20, 12, 3, 30, 10, 3))]
    #clear_etcd_server()
    generate_tuples()


def generate_minimum_tuples():
    tuples = [(configTuple(100, 40, 25, 5, 60, 35, 10)),
              (configTuple(100, 40, 25, 5, 60, 35, 20)),
              (configTuple(100, 40, 25, 5, 60, 40, 10)),
              (configTuple(100, 30, 20, 5, 60, 35, 10)),
              (configTuple(100, 30, 20, 5, 25, 60, 10))]
    return tuples


def generate_basic_tuples():
    tuples = []
    for n in range(20, 30, 50):
        for k in range(int(n/5), int(4*n/5), int(n/5)):
            alphas = [int(k/2+1) , int(2*k/3)]
            for alpha in alphas:
                betas = [2, 3]
                for beta in betas:
                    reds = [int(4*n/10) , int(7*n/10)]
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
