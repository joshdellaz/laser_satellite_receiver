import matplotlib.pyplot as plt
from numpy import genfromtxt
import numpy as np
import csv

#add metadata functionality (title) via new csv
file = open('title.csv')
reader = csv.reader(file)
[title, x, y, scale] = next(reader)
x = int(x)
y = int(y)
scale = int(scale)
xlist = np.linspace(0, x*scale, x)
ylist = np.linspace(0, y*scale, y)
X, Y = np.meshgrid(xlist, ylist)
Z = np.empty((x,y))

data = genfromtxt('output.csv', delimiter=',')

for i in range(x):
    for j in range(y):
        Z[i,j] = data[i*y + j]

cp = plt.contourf(X, Y, Z)
plt.colorbar(cp)
plt.title(title)
plt.show()