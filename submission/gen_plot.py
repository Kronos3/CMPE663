import matplotlib.pyplot as plt
import matplotlib

import pandas as pd
data = pd.read_csv('report_data.csv', header=None)
data = pd.DataFrame(data)

x = data[0]
plt.plot(x, data[1], 'g-', label="Reference")
first = plt.plot(x, data[2], 'r-', label="1st iteration")
second = plt.plot(x, data[3], 'b-', label="10th iteration")
plt.legend(loc="upper left", fontsize=20)
plt.show()
