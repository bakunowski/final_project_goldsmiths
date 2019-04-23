import json
import numpy as np
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.models import model_from_json

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

# load json and create model
json_file = open('model.json', 'r')
loaded_model_json = json_file.read()
json_file.close()
loaded_model = model_from_json(loaded_model_json)
# load weights into new model
loaded_model.load_weights("model.h5")
print("Loaded model from disk")

optimizer = tf.train.GradientDescentOptimizer(0.1)

# evaluate loaded model on test data
loaded_model.compile(loss='mean_squared_error', optimizer=optimizer, metrics=['mean_squared_error', 'accuracy'])
loss, mse, score = loaded_model.evaluate(X_test, y_test, verbose=0)
print('Test accuracy:', score*100)
