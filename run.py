import os
import subprocess
import random
import sys
import getopt

def run_nodes(n, k, alpha, beta):
  num_of_nodes = int(n)
  processes = []

  general_config_file = open("files/general_config.txt", "w")
  general_config_file.write(str(num_of_nodes) + "\n")
  general_config_file.write(k + "\n")
  general_config_file.write(alpha + "\n")
  general_config_file.write(beta + "\n")
  general_config_file.close()

  for i in range(1, num_of_nodes + 1):
    file_name = "files/config_" + str(i) + ".txt"

    colors = ['R', 'B', 'U']
    num = random.randint(0, 2)
    byzantine = random.randint(0, 1)

    config_file = open(file_name, "w")
    config_file.write(str(i) + "\n")
    config_file.write(colors[num] + "\n")
    config_file.write(str(byzantine) + "\n")
    config_file.close()

  for i in range(1, num_of_nodes + 1):
    p = subprocess.Popen(["cmake/build/node", f"{i}", f"{n}", f"{k}", f"{alpha}", f"{beta}"])
    processes.append(p)
    #p.communicate()

  for p in processes:
    p.wait()

def main(argv):
  n = ''
  k = ''
  alpha = ''
  beta = ''

  try:
    opts, args = getopt.getopt(argv,"n:k:a:b:",["alpha=","beta="])
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
  run_nodes(n=n, k=k, alpha=alpha, beta=beta)

if __name__ == "__main__":
  main(sys.argv[1:])

