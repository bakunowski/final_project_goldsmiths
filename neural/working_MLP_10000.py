import json
import numpy as np
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf
from tensorflow.keras import layers

np.set_printoptions(suppress=True)

scaler = MinMaxScaler()

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/newest_data_10000_examples/MFCC.json'
) as f:
    d2 = json.load(f)

mfcc = np.array(d2["lowlevel"]["mfcc"])
scaler.fit(mfcc)
mfcc_norm = scaler.transform(mfcc)
# normalized between 0 and 1 and reshaped to 45 frames x 13 values
mfcc_norm_reshaped = mfcc_norm.reshape(10000, 45, 13)

with open('/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/newest_data_10000_examples/parameters.json') as f:
     d1 = json.load(f)
 
duration = np.array(d1["parameters"]["duration"]).reshape(10000, 1)
numberOfGrains = np.array(d1["parameters"]["numberOfGrains"]).reshape(10000, 1)
pitch = np.array(d1["parameters"]["pitch"]).reshape(10000, 1)
position = np.array(d1["parameters"]["position"]).reshape(10000, 1)
spread = np.array(d1["parameters"]["spread"]).reshape(10000, 1)

parameters = np.concatenate((duration, numberOfGrains, pitch, position, spread), axis=1)

scaler.fit(parameters)
# normalized between 0 and 1
parameters_norm = scaler.transform(parameters)

X_train = mfcc_norm_reshaped[:8000]
y_train = parameters_norm[:8000]

X_test = mfcc_norm_reshaped[8000:10000]
y_test = parameters_norm[8000:10000]

print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)
print("Test dimensions", X_test.shape)
print("Test labels dimensions", y_test.shape)

model = tf.keras.Sequential([
    layers.Flatten(input_shape=(45,13)),
    layers.Dense(585, activation='relu'),
    layers.Dense(128, activation='relu'),
    layers.Dense(5)])

optimizer = tf.train.GradientDescentOptimizer(0.1)

model.compile(loss='mean_squared_error',
              optimizer=optimizer,
              metrics=['mean_squared_error', 'accuracy'])

model.summary()

model.fit(X_train, y_train, epochs=10, batch_size=64, verbose=1)

loss, mse, test_acc = model.evaluate(X_test, y_test, verbose=1)
print('Test accuracy:', test_acc)

model.save('test_1st_model.h5', include_optimizer=False)

# # serialize model to JSON
# model_json = model.to_json()
# with open("model.json", "w") as json_file:
#     json_file.write(model_json)
# # serialize weights to HDF5
# model.save_weights("model.h5")
# print("Saved model to disk")
