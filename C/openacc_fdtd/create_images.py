import numpy as np
from PIL import Image
import sys

nx = int(sys.argv[1])
ny = nx

dt = int(sys.argv[3])

nt = int(sys.argv[2]) // dt + 1

data = np.fromfile("result.grd", dtype=np.float64).reshape(nt, 3, ny+17, nx+17)

data = data[:,0,::-1,:]
data = data[:,9:-8,9:-8]

for it in range(nt):
	rgb = np.empty((ny, nx, 3), dtype=np.uint8)
	rgb[:,:,0] = np.interp(data[it,:,:], [-50,0,50,100], [0, 255, 255, 128])
	rgb[:,:,1] = np.interp(data[it,:,:], [-50,0,50], [0, 255, 0])
	rgb[:,:,2] = np.interp(data[it,:,:], [-100,-50,0,50], [128, 255, 255, 0])
	Image.fromarray(rgb).save(f"images/{it*dt:04d}.png")

