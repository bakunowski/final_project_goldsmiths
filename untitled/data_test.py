import pandas as pd
import numpy as np
from sklearn import linear_model
import matplotlib.pyplot as plt
data = pd.read_json('data.json', orient='split')

mfcc = pd.read_json((data['mfcc']).to_json())
mfcc_transposed = mfcc.T
#print(mfcc_transposed)
#print(mfcc_transposed.shape)

parameters = pd.read_json((data['parameters']).to_json(), orient='columns')
parameters_transposed = parameters.T
#print(mfcc_transposed)
#print(parameters_transposed.shape)

regr = linear_model.LinearRegression()
regr.fit(mfcc_transposed, parameters_transposed)

parameters_prediction = regr.predict(mfcc_transposed)
print(parameters_prediction)

# The mean square error
#np.mean((regr.predict(mfcc_transposed) - parameters_transposed) ** 2)

# Explained variance score: 1 is perfect prediction
# and 0 means that there is no linear relationship
# between X and y.
#score = regr.score(mfcc_transposed, parameters_prediction)

