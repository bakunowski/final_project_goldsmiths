import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf

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

np.random.shuffle(mfcc)
np.random.shuffle(numberOfGrains)

X_train = mfcc[0:5000]
# y_train = parameters
y_train = numberOfGrains[0:5000]

X_test = mfcc[5001:6299]
y_test = numberOfGrains[5001:6299]

np.set_printoptions(suppress=True)
print("Train dimensions", X_train.shape)
print("Train labels dimensions", y_train.shape)

# set learning variables
learning_rate = 0.0001
epochs = 100
batch_size = 64

# set some variables
x = tf.placeholder(tf.float32, [None, 13], name='x')  # 13 feature
y = tf.placeholder(tf.float32, [None, 1], name='y')  # 5 outputs

# hidden layer 1
W1 = tf.Variable(tf.truncated_normal([13, 10], stddev=0.03), name='W1')
b1 = tf.Variable(tf.truncated_normal([10]), name='b1')

# hidden layer 2
W2 = tf.Variable(tf.truncated_normal([10, 1], stddev=0.03), name='W2')
b2 = tf.Variable(tf.truncated_normal([1]), name='b2')

# Activations, outputs
# output hidden layer 1
hidden_out = tf.nn.relu(tf.add(tf.matmul(x, W1), b1))

# total output
y_ = tf.nn.relu(tf.add(tf.matmul(hidden_out, W2), b2))

# Loss Function
mse = tf.losses.mean_squared_error(y, y_)
cost = tf.reduce_mean(tf.square(y_ - y))

# Optimizer
optimizer = tf.train.GradientDescentOptimizer(
    learning_rate=learning_rate).minimize(mse)

# Initialize, Accuracy and Run
# initialize variables
init_op = tf.global_variables_initializer()

# accuracy for the test set
accuracy = tf.reduce_mean(tf.square(tf.subtract(
    y, y_)))  # or could use tf.losses.mean_squared_error

# run
with tf.Session() as sess:
    sess.run(init_op)
    total_batch = int(len(y_train) / batch_size)
    for epoch in range(epochs):
        avg_cost = 0
        for i in range(total_batch):
            batch_x, batch_y = X_train[i * batch_size:min(i * batch_size + batch_size,len(X_train)),:],\
                               y_train[i * batch_size:min(i * batch_size + batch_size, len(y_train)), :]
            _, c = sess.run([optimizer, cost],feed_dict={x: batch_x,y: batch_y})
            avg_cost += c / total_batch
        if epoch % 10 == 0:
            print('Epoch:', (epoch+1), 'cost =', '{:.3f}'.format(avg_cost))
    print(sess.run(cost, feed_dict={x: X_test, y: y_test}))
