import json
import numpy as np
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf
from tensorflow.keras import layers

np.set_printoptions(suppress=True)

scaler = MinMaxScaler()

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/newest_data_1000_examples/MFCC.json'
) as f:
    d2 = json.load(f)

mfcc = np.array(d2["lowlevel"]["mfcc"])
scaler.fit(mfcc)
mfcc_norm = scaler.transform(mfcc)
# normalized between 0 and 1 and reshaped to 45 frames x 13 values
mfcc_norm_reshaped = mfcc_norm.reshape(1000, 45, 13)

with open('/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/newest_data_1000_examples/parameters.json') as f:
    d1 = json.load(f)

duration = np.array(d1["parameters"]["duration"]).reshape(1000, 1)
numberOfGrains = np.array(d1["parameters"]["numberOfGrains"]).reshape(1000, 1)
pitch = np.array(d1["parameters"]["pitch"]).reshape(1000, 1)
position = np.array(d1["parameters"]["position"]).reshape(1000, 1)
spread = np.array(d1["parameters"]["spread"]).reshape(1000, 1)

parameters = np.concatenate((duration, numberOfGrains, pitch, position, spread), axis=1)

scaler.fit(parameters)
# normalized between 0 and 1
parameters_norm = scaler.transform(parameters)

X_train = mfcc_norm_reshaped
y_train = parameters_norm

np.set_printoptions(suppress=True)
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

model = tf.keras.Sequential([
    layers.Flatten(input_shape=(45,13)),
    layers.Dense(585, activation='relu'),
    layers.Dense(128, activation='relu'),
    layers.Dense(5)])

optimizer = tf.train.GradientDescentOptimizer(0.1)

model.compile(loss='mean_squared_error',
              optimizer=optimizer,
              metrics=['mean_squared_error'])

model.summary()

example_batch = X_train[:10]
example_result = model.predict(example_batch)
print(example_result)