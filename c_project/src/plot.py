import matplotlib.pyplot as plt
from numpy import genfromtxt

data = genfromtxt('output.csv', delimiter=',')
plt.stem(data)
plt.show()