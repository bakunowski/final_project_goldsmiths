import json
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd

print(tf.VERSION)
print(tf.keras.__version__)

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/MFCC_new.json'
) as f:
    d2 = json.load(f)

mfcc = np.array(d2["lowlevel"]["mfcc"])[0:38700].reshape(860, 45, 13)

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/parameters_new.json'
) as f:
    d1 = json.load(f)

duration = np.array(d1["parameters"]["duration"])[0:860].reshape(860, 1)
numberOfGrains = np.array(d1["parameters"]["numberOfGrains"])[0:860].reshape(860, 1)
pitch = np.array(d1["parameters"]["pitch"])[0:860].reshape(860, 1)
position = np.array(d1["parameters"]["position"])[0:860].reshape(860, 1)
spread = np.array(d1["parameters"]["spread"])[0:860].reshape(860, 1)

parameters = np.concatenate(
    (duration, numberOfGrains, pitch, position, spread), axis=1)

print(mfcc.shape)

print(parameters.shape)

X_train = mfcc[:800]

y_train = pitch[:800]
print(len(np.unique(pitch)))

X_test = mfcc[800:860]
y_test = pitch[800:860]

np.set_printoptions(suppress=True)
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

model = tf.keras.Sequential([
    layers.Flatten(input_shape=(45, 13)),
    # Adds a densely-connected layer with 64 units to the model:
    layers.Dense(512, activation='relu'),
    # Add another:
    layers.Dense(24, activation='softmax')])
    # Add a softmax layer with 10 output units:
    #layers.Dense(4)])

# optimizer = tf.keras.optimizers.RMSprop(0.001)
optimizer = tf.train.AdamOptimizer(0.005)

model.compile(loss='mean_squared_error',
              optimizer=optimizer,
              metrics=['mean_absolute_error', 'mean_squared_error'])

model.summary()

# example_batch = X_train[:10]
# example_result = model.predict(example_batch)
# print(example_result)

model.fit(X_train, y_train, epochs=1000, batch_size=50, callbacks=[tf.keras.callbacks.EarlyStopping(monitor='mean_absolute_error', patience=10)])

loss, mae, mse = model.evaluate(X_test, y_test, verbose=0)

print("Testing set Mean Abs Error: {:5.2f} MPG".format(mae))

test_predictions = model.predict(X_test, batch_size=10)
np.set_printoptions(precision=1, suppress=True)
for i in range(len(test_predictions)):
    print(np.argmax(test_predictions[i]))
print(y_test)

# def norm(x):
#     return (x - np.mean(x)) / np.std(x)
#
#
# def build_model():
#     model = tf.keras.Sequential([
#         layers.Flatten(input_shape=(13, 45)),
#         # add a densly-connected layer with 64 units to the model:
#         layers.Dense(56, activation=tf.nn.relu),
#         # add another
#         layers.Dense(24, activation=tf.nn.relu),
# #        layers.Dense(10, activation=tf.nn.relu),
# #        layers.Dense(10, activation=tf.nn.relu),
#
#         # no activation function here, because regression
#         # 5 as in number of outputs we'd like to have
# #        layers.Dense(1)
#     ])
#
#     optimizer = tf.keras.optimizers.Adam()
#
#     model.compile(optimizer=optimizer,
#                   loss='sparse_categorical_crossentropy',
#                   metrics=['accuracy'])
#
#     return model
#
#
# model = build_model()
#
# model.summary()
#
# # example_batch = norm(X_train[:10])
# # example_result = model.predict(example_batch)
# # print(example_result)
#
# # early_stop = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=10)
#
# EPOCHS = 1000
#
# model.fit(X_train,
#           y_train,
#           epochs=50,
#           verbose=1)
# # ,
# #          callbacks=[early_stop])
#
# # hist = pd.DataFrame(history.history)
# # hist['epoch'] = history.epoch
# # hist.tail()
# #
# # loss, mae, mse = model.evaluate(norm(X_test), y_test, verbose=1)
# #
# # print("Testing set Mean Abs Error: {:5.2f} MPG".format(mae))
#
#
# # model.evaluate(X_test, y_test, batch_size=32)
#
# # # output of the last layer for new data
#
# test_loss, test_acc = model.evaluate(X_test, y_test)
#
# print('Test accuracy:', test_acc)
#
# test_pred = model.predict(X_test)
# print(np.argmax(test_pred[0]), y_test[0])
# print(np.argmax(test_pred[1]), y_test[1])
# print(np.argmax(test_pred[2]), y_test[2])
# print(np.argmax(test_pred[3]), y_test[3])
