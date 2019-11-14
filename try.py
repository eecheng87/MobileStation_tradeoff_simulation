import matplotlib.pyplot as plt
import numpy as np
t = np.arange(1,86401)
p1 = np.loadtxt("policy1.txt", delimiter="\n")
p2 = np.loadtxt("policy2.txt", delimiter="\n")
p3 = np.loadtxt("policy3.txt", delimiter="\n")
plt.plot(t,p1)
plt.plot(t,p2)
plt.plot(t,p3)
plt.show()
