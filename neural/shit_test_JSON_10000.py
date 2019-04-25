import json
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
from sklearn.preprocessing import MinMaxScaler

print(tf.VERSION)
print(tf.keras.__version__)
np.set_printoptions(suppress=True)

scaler = MinMaxScaler()

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/MFCC_10000.json'
) as f:
    d2 = json.load(f)

mfcc = np.array(d2["lowlevel"]["mfcc"])[:371430]
# .reshape(8254, 45, 13)
scaler.fit(mfcc)
mfcc_norm = scaler.transform(mfcc)
# normalized between 0 and 1 and reshaped to 45 frames x 13 values
mfcc_norm_reshaped = mfcc_norm.reshape(8254, 45, 13)

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/parameters_10000.json'
) as f:
    d1 = json.load(f)

duration = np.array(d1["parameters"]["duration"])[:8254].reshape(8254, 1)
numberOfGrains = np.array(d1["parameters"]["numberOfGrains"])
pitch = np.array(d1["parameters"]["pitch"])
position = np.array(d1["parameters"]["position"])
spread = np.array(d1["parameters"]["spread"])
# parameters = np.concatenate((duration, numberOfGrains, pitch, position, spread), axis=1)

scaler.fit(duration)
# normalized between 0 and 1
duration_norm = scaler.transform(duration)

X_train = mfcc_norm_reshaped[:8000]
y_train = duration_norm[:8000]

X_test = mfcc_norm_reshaped[8000:8254]
y_test = duration_norm[8000:8254]

np.set_printoptions(suppress=True)
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

model = tf.keras.Sequential([
    layers.Flatten(input_shape=(45,13)),
    layers.Dense(585, activation='relu'),
    layers.Dense(128, activation='relu'),
    layers.Dense(1)])

optimizer = tf.train.GradientDescentOptimizer(0.1)

model.compile(loss='mean_squared_error',
              optimizer=optimizer,
              metrics=['mean_absolute_error', 'mean_squared_error'])

model.summary()

#example_batch = X_train[:10]
#example_result = model.predict(example_batch)
#print(example_result)