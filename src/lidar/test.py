import numpy as np
x_range = np.arange(start=20, stop=160)
y_range = np.arange(start=60, stop=120)
raw_data = np.empty([x_range.shape[0]*y_range.shape[0], 3], dtype=np.int32)
raw_data[:,0] = np.tile(x_range, y_range.shape[0])
raw_data[:,1] = np.repeat(y_range, x_range.shape[0])
raw_data[:,2] = 0
print(raw_data)
