# -*- coding: utf-8 -*-
"""DustSensor.ipynb

Automatically generated by Colab.

Original file is located at
    https://colab.research.google.com/drive/192Sp5RvkcE4eap49lDc2YQqGrFWeKLtR
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LinearRegression
from google.colab import files
from io import BytesIO

uploaded = files.upload()
file_name = list(uploaded.keys())[0]
df = pd.read_csv(BytesIO(uploaded[file_name]))
df['timestamp'] = pd.to_datetime(df['timestamp'], format='%a %b %d %Y %H:%M:%S GMT+0000 (Coordinated Universal Time)')

print(f"データの総数: {len(df)}")

x = (df['timestamp'] - df['timestamp'].min()).dt.total_seconds().values.reshape(-1, 1)
y = df['Ratio'].values

model = LinearRegression()
model.fit(x, y)

plt.figure(figsize=(12, 6))
plt.scatter(df['timestamp'], df['Ratio'])
plt.plot(df['timestamp'], model.predict(x), color='red')

plt.xlabel('Timestamp')
plt.ylabel('Ratio')

slope = model.coef_[0]
intercept = model.intercept_

print(f"回帰直線の方程式: y = {slope:.6f}x + {intercept:.6f}")
print(f"1時間あたりの平均増加率: {slope * 3600:.6f}")