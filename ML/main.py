import pandas as pd
from sklearn import linear_model
from sklearn.model_selection import train_test_split
from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client
import numpy as np

import argparse

data = pd.read_json('dataset.json', orient='split')

mfcc = pd.read_json((data['mfcc']).to_json())
mfcc_transposed = mfcc.T
mfcc_ready = mfcc_transposed.values

parameters = pd.read_json((data['parameters']).to_json(), orient='columns')
parameters_transposed = parameters.T
parameters_ready = parameters_transposed.values

#X_train, X_Test, y_train, y_test = train_test_split(mfcc_ready, parameters_ready, test_size=0.2)

regr = linear_model.LinearRegression()
#regr.fit(X_train, y_train)
regr.fit(mfcc_ready, parameters_ready)

def mfcc_analyse(unused_addr, args0, args1, args2, args3, args4, args5, args6, args7, args8, args9, args10, args11, args12):
    values = np.array([args0, args1, args2, args3, args4, args5, args6, args7, args8, args9, args10, args11, args12])
    var_test = np.expand_dims(values, axis=0)
    parameters_prediction = regr.predict(var_test)
    result = np.squeeze(parameters_prediction, axis=0)
    print(result)
    client.send_message("/values", result)

if __name__ == "__main__":

    ip = "127.0.0.1"
    inPort = 6448
    sendPort = 12000

    #sending OSC
    client = udp_client.SimpleUDPClient(ip, sendPort)

    #catches osc messages
    dispatcher = dispatcher.Dispatcher()
    dispatcher.map("/test", mfcc_analyse)

    #setup server to listen for osc messages
    server = osc_server.ThreadingOSCUDPServer((ip, inPort), dispatcher)
    print("servering on {}".format(server.server_address))
    server.serve_forever()