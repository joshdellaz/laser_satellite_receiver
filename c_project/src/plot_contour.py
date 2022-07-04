import matplotlib.pyplot as plt
from numpy import genfromtxt
import numpy as np
import csv

#add metadata functionality (title) via new csv
file = open('title.csv')
title = next(csv.reader(file))
x = next(csv.reader(file))
y = next(csv.reader(file))
scale = next(csv.reader(file))
xlist = np.linspace(0, x*scale, scale)
ylist = np.linspace(0, y*scale, scale)
X, Y = np.meshgrid(xlist, ylist)

data = genfromtxt('output.csv', delimiter=',')

cp = plt.contourf(X, Y, data)
plt.colorbar(cp)
plt.title(title)
plt.show()