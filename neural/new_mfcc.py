import json
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers

with open(
        '/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/neural/MFCC_new.json'
) as f:
    d2 = json.load(f)

# mfcc = np.array(d2["lowlevel"]["mfcc"])[0:36000].reshape(800, 45, 13)
# test = np.array(d2["lowlevel"]["mfcc"])

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

# print(test.shape)
print(mfcc.shape)

# print(test.shape)
print(parameters.shape)

X_train = mfcc
# y_train = parameters
y_train = parameters

# X_test = mfcc_conv[5001:6299]
# y_test = parameters_conv[5001:6299]

np.set_printoptions(suppress=True)
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)


def norm(x):
    return (x - np.mean(x)) / np.std(x)


def build_model():
    model = tf.keras.Sequential([
        # add a densly-connected layer with 64 units to the model:
        layers.Dense(256, activation=tf.nn.relu, input_shape=(45, 13)),
        # add another
        layers.Dense(256, activation=tf.nn.relu),
#        layers.Dense(10, activation=tf.nn.relu),
#        layers.Dense(10, activation=tf.nn.relu),

        # no activation function here, because regression
        # 5 as in number of outputs we'd like to have
        layers.Dense(1)
    ])

    optimizer = tf.keras.optimizers.Adam(0.005)

    model.compile(optimizer=optimizer,
                  loss='mean_squared_error',
                  metrics=['mean_absolute_error', 'mean_squared_error'])

    return model


model = build_model()

model.summary()

# print(X_train[:1])
example_batch = norm(X_train[:1])
example_result = model.predict(example_batch)
print(example_result)

early_stop = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=10)

EPOCHS = 1000

# model.fit(norm(X_train),
#           y_train,
#           batch_size=64,
#           epochs=EPOCHS,
#           validation_split=0.2,
#           verbose=1,
#           callbacks=[early_stop])

# hist = pd.DataFrame(history.history)
# hist['epoch'] = history.epoch
# hist.tail()
#
# loss, mae, mse = model.evaluate(norm(X_test), y_test, verbose=1)
#
# print("Testing set Mean Abs Error: {:5.2f} MPG".format(mae))


# model.evaluate(X_test, y_test, batch_size=32)

# # output of the last layer for new data
# test_pred = model.predict(norm(X_test))
# print(test_pred, y_test)

