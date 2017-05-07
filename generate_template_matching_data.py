import numpy as np
from matplotlib.pyplot import imshow
import matplotlib.pyplot as plt
import os

x = [100]
y = [100]
images = 30
samples=100
sigma = 30

path = str(os.getcwd() + "/images")
if not os.path.exists(path):
    os.makedirs(path)

for j in range(images):
    rx = x[j] + 1.*np.random.randint(-sigma,sigma)
    if rx < 200-sigma and rx > sigma:
        x.append(rx)
    else:
        x.append(x[j])
    ry = y[j] + 1.*np.random.randint(-sigma,sigma)
    if ry < 200-sigma and ry > sigma:
        y.append(ry)
    else:
        y.append(y[j])

for i in range(images):
    data = np.zeros((200,200),dtype=int)
    mean = [x[i], y[i]]
    cov = [[5, 0], [0, 5]]
    dist_x, dist_y = np.random.multivariate_normal(mean, cov, samples).T
    plt.figure(figsize=(6,6));
    plt.axis('off')
    plt.scatter(dist_x, dist_y,color='b',alpha=0.2);
    plt.xlim(0,200)
    plt.ylim(0,200)

    filename = str(path + "/" + str(i) + ".png")
    plt.savefig(filename, bbox_inches='tight', pad_inches=0)
