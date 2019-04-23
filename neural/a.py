import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf
from tensorflow.keras import layers

test_array = []
mfcc = pd.read_json('MFCC.json')
mfcc_np = np.asarray(mfcc)

c = np.array(list(mfcc_np[:,0]))

parameters = pd.read_json('parameters.json', orient='index')

duration = pd.read_json((parameters['duration']).to_json())
duration_np = np.array(duration)

number_of_grains = pd.read_json((parameters['numberOfGrains']).to_json())
number_of_grains_np = np.array(number_of_grains)

pitch = pd.read_json((parameters['pitch']).to_json())
pitch_np = np.array(pitch)

position = pd.read_json((parameters['position']).to_json())
position_np = np.array(position)

spread = pd.read_json((parameters['spread']).to_json())
spread_np = np.array(spread)

test = np.reshape(duration_np, (6299, 1))
test2 = np.reshape(number_of_grains_np, (6299, 1))
test3 = np.reshape(pitch_np, (6299, 1))
test4 = np.reshape(position_np, (6299, 1))
test5 = np.reshape(spread_np, (6299, 1))

# X_train = mfcc_np
X_train = c[0:6000]
X_test = c[6000:6299]
a = np.concatenate((test, test2, test3, test4, test5), axis=1)
y_train = a[0:6000]
y_test = a[6000:6299]

print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

model = tf.keras.Sequential([
    # Adds a densely-connected layer with 64 units to the model:
    layers.Dense(512, activation='relu', input_shape=(13,)),
    # Add another:
    layers.Dense(256, activation='relu'),
    # Add a softmax layer with 10 output units:
    layers.Dense(5)])

# optimizer = tf.keras.optimizers.RMSprop(0.001)
optimizer = tf.train.AdamOptimizer(0.005)

model.compile(loss='mean_squared_error',
              optimizer=optimizer,
              metrics=['mean_absolute_error', 'mean_squared_error'])

model.summary()

# example_batch = X_train[:10]
# example_result = model.predict(example_batch)
# print(example_result)

model.fit(X_train, y_train, epochs=1000, batch_size=500, callbacks=[tf.keras.callbacks.EarlyStopping(monitor='mean_absolute_error', patience=10)])

loss, mae, mse = model.evaluate(X_test, y_test, verbose=1)

print("Testing set Mean Abs Error: {:5.2f} MPG".format(mae))

test_predictions = model.predict(X_test, batch_size=10)
np.set_printoptions(precision=1, suppress=True)
print(test_predictions)
print(y_test)
