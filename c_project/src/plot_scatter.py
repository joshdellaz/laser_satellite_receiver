import matplotlib.pyplot as plt
from numpy import genfromtxt
import csv

#add metadata functionality (title) via new csv
file = open('title.csv')
title = next(csv.reader(file))

data = genfromtxt('output.csv', delimiter=',')
plt.stem(data)
plt.title(title)
plt.show()