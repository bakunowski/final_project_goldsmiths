import json
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd

print(tf.VERSION)
print(tf.keras.__version__)

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/test.json'
) as f:
    json_data = json.load(f)

mfcc = np.array(json_data["mfcc"])

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/parameters.json'
) as f:
    d2 = json.load(f)

duration = np.array(d2["parameters"]["duration"]).reshape(6299, 1)
numberOfGrains = np.array(d2["parameters"]["numberOfGrains"]).reshape(6299, 1)
pitch = np.array(d2["parameters"]["pitch"]).reshape(6299, 1)
position = np.array(d2["parameters"]["position"]).reshape(6299, 1)
spread = np.array(d2["parameters"]["spread"]).reshape(6299, 1)

parameters = np.concatenate(
    (duration, numberOfGrains, pitch, position, spread), axis=1)

mfcc_shuffled = np.random.shuffle(mfcc)
parameters_shuffled = np.random.shuffle(parameters)
numberOfGrains_shuffled = np.random.shuffle(numberOfGrains)

np.random.shuffle(mfcc)
np.random.shuffle(numberOfGrains)

X_train = mfcc[0:5000]

y_train = numberOfGrains[0:5000]

X_test = mfcc[5001:6299]
y_test = numberOfGrains[5001:6299]

print(mfcc[0][0])
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

import keras
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.utils import to_categorical

model = Sequential()
model.add(Conv2D(32, kernel_size=(2, 2), activation='relu', input_shape=(13, )))
model.add(MaxPooling2D(pool_size=(2, 2)))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(128, activation='relu'))
model.add(Dropout(0.25))
model.add(Dense(1))
model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=keras.optimizers.Adadelta(),
              metrics=['accuracy'])

model.fit(X_train, y_train, batch_size=100, epochs=200, verbose=1, validation_data=(X_test, y_test))
