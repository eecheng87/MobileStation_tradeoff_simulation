import matplotlib.pyplot as plt
import numpy as np
t = np.arange(1,86401)
p1 = np.loadtxt("policy1.txt", delimiter="\n")
p2 = np.loadtxt("policy2.txt", delimiter="\n")
p3 = np.loadtxt("policy3.txt", delimiter="\n")
p4 = np.loadtxt("policy4.txt", delimiter="\n")

plt.plot(t,p1,label="p1")
plt.plot(t,p2,label="p2")
plt.plot(t,p3,label="p3")
plt.plot(t,p4,label="p4")
plt.legend(loc='upper left')
plt.show()
