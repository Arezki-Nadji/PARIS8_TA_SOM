from PIL import Image
import matplotlib.pyplot as plt
import numpy as np

filename = 'outresult.txt'
f = open(filename,"r")
data =f.read()
data =data.split(",")
data.pop()
data_int = np.zeros((6,10))
data =np.reshape(data,(6,10))
for i in range(6):
	for j in range(10):
		data_int[i][j] = float(data[i][j])

plt.imshow(data_int)
plt.show()
